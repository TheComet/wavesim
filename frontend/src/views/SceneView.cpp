#include "frontend/views/SceneView.hpp"
#include "frontend/models/Camera.hpp"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/math.h"

#include <QWidget>
#include <QResizeEvent>
#include <QTimer>
#include <QApplication>

#define RENDER_PASS_SHADING 0  // Default forward rendered geo with simple shading
#define RENDER_PASS_ID      1  // ID buffer for picking
#define RENDER_PASS_BLIT    2  // Blit GPU render target to CPU texture

namespace frontend {

struct PosColorVertex
{
    float m_x;
    float m_y;
    float m_z;
    uint32_t m_abgr;

    static void init()
    {
        ms_decl
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
            .end();
    };

    static bgfx::VertexDecl ms_decl;
};

bgfx::VertexDecl PosColorVertex::ms_decl;


static PosColorVertex s_cubeVertices[] =
{
    {-1.0f,  1.0f,  1.0f, 0xff000000 },
    { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
    { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    {-1.0f,  1.0f, -1.0f, 0xffff0000 },
    { 1.0f,  1.0f, -1.0f, 0xffff00ff },
    {-1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t s_cubeTriList[] =
{
    0, 1, 2, // 0
    1, 3, 2,
    4, 6, 5, // 2
    5, 6, 7,
    0, 2, 4, // 4
    4, 2, 6,
    1, 5, 3, // 6
    5, 7, 3,
    0, 4, 1, // 8
    4, 5, 1,
    2, 3, 6, // 10
    6, 3, 7,
};

static bgfx::VertexBufferHandle m_vbh;
static bgfx::IndexBufferHandle m_ibh;
static bgfx::ProgramHandle m_program;

// ----------------------------------------------------------------------------
SceneView::SceneView(QScreen* screen) :
    QWindow(screen),
    resizeTimer_(nullptr),
    camera_(nullptr)
{
    initGraphics();
    resizeTimer_ = new QTimer(this);
    camera_ = new Camera(this);
    connect(resizeTimer_, SIGNAL(timeout()), this, SLOT(onResizeTimerTimeout()));

    m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), PosColorVertex::ms_decl);
    m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList)));

    /*bgfx::ShaderHandle vsh =
    m_program = bgfx::createProgram(vsh, fsh, true);*/
}

// ----------------------------------------------------------------------------
SceneView::SceneView(QWindow* parent) :
    QWindow(parent)
{
    initGraphics();
}

// ----------------------------------------------------------------------------
SceneView::~SceneView()
{
    bgfx::destroy(m_ibh);
    bgfx::destroy(m_vbh);
    bgfx::shutdown();
}

// ----------------------------------------------------------------------------
void SceneView::initGraphics()
{
    setSurfaceType(QSurface::OpenGLSurface);

    bgfx::Init init;
    init.resolution.width = 1;  // we get resized anyway
    init.resolution.height = 1;
    init.resolution.reset = BGFX_RESET_VSYNC;
    init.platformData.nwh = reinterpret_cast<void*>(winId());
    bgfx::init(init);

    // Set up screen clears
    bgfx::setViewClear(0
        , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
        , 0x007700ff
        , 1.0f
        , 0
        );
}

// ----------------------------------------------------------------------------
void SceneView::resizeEvent(QResizeEvent* e)
{
    // Without the timer we get a horrible flickering mess. This smooths it
    // out a bit.
    resizeTimer_->start(40);
}

// ----------------------------------------------------------------------------
void SceneView::mouseMoveEvent(QMouseEvent* e)
{
    camera_->updateMouse(e->pos());
}

// ----------------------------------------------------------------------------
void SceneView::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::MiddleButton)
    {
        Camera::Action action = Camera::ROTATE;
        if (QApplication::keyboardModifiers() == Qt::ShiftModifier)
            action = Camera::TRANSLATE;
        else if (QApplication::keyboardModifiers() == Qt::ControlModifier)
            action = Camera::ZOOM;

        camera_->beginAction(action, e->pos());
    }
}

// ----------------------------------------------------------------------------
void SceneView::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::MiddleButton)
        camera_->endAction();
}

// ----------------------------------------------------------------------------
void SceneView::onResizeTimerTimeout()
{
    if (width() == 0 || height() == 0)
        return;
    bgfx::reset(width(), height(), BGFX_RESET_VSYNC);
    draw();
}

// ----------------------------------------------------------------------------
void SceneView::draw()
{
    float viewMat[16];
    float projMat[16];
    camera_->getViewMatrix(viewMat);
    bx::mtxProj(projMat, 60.0f, float(width()) / float(height()), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(RENDER_PASS_SHADING, viewMat, projMat);
    bgfx::setViewRect(RENDER_PASS_SHADING, 0, 0, width(), height());

    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);

    uint64_t state = 0
        | BGFX_STATE_WRITE_R
        | BGFX_STATE_WRITE_G
        | BGFX_STATE_WRITE_B
        | BGFX_STATE_WRITE_A
        | BGFX_STATE_WRITE_Z
        | BGFX_STATE_DEPTH_TEST_LESS
        | BGFX_STATE_CULL_CW
        | BGFX_STATE_MSAA
        | BGFX_STATE_PT_TRISTRIP
        ;
    bgfx::setState(state);
    bgfx::submit(RENDER_PASS_SHADING, m_program);

    bgfx::frame();
}

}
