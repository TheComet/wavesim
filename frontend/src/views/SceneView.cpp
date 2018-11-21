#include "frontend/views/SceneView.hpp"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"

#include <QWidget>

#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#endif

namespace frontend {

// ----------------------------------------------------------------------------
SceneView::SceneView(QScreen* screen) :
    QWindow(screen)
{
    initGraphics();
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
    bgfx::shutdown();
#ifdef Q_OS_LINUX
    XCloseDisplay(reinterpret_cast<::Display*>(X11Display_));
#endif
}

// ----------------------------------------------------------------------------
void SceneView::initGraphics()
{
    setSurfaceType(QSurface::OpenGLSurface);

    setWidth(640);
    setHeight(480);


    bgfx::Init init;
    init.resolution.width = width();
    init.resolution.height = height();
    init.resolution.reset = BGFX_RESET_VSYNC;
    init.platformData.nwh = reinterpret_cast<void*>(winId());
#ifdef Q_OS_LINUX
    X11Display_ = XOpenDisplay(NULL);
    init.platformData.ndt = reinterpret_cast<void*>(X11Display_);
#endif
    bgfx::setPlatformData(init.platformData);
    bgfx::init(init);
    bgfx::reset(width(), height(), BGFX_RESET_VSYNC);

    // Set up screen clears
    bgfx::setViewClear(0
        , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
        , 0x303030ff
        , 1.0f
        , 0
        );
}

}
