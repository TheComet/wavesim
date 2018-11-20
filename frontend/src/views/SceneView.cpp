#include "frontend/views/SceneView.hpp"
#include <Qt3DCore/Qentity>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QTransform>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>

#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DExtras/QOrbitCameraController>

#include "frontend/models/SceneLoader.hpp"

using namespace frontend;

// ----------------------------------------------------------------------------
SceneView::SceneView(QScreen* screen) :
    Qt3DWindow(screen)
{
    // Root entity
    rootEntity_ = new Qt3DCore::QEntity;

    // Camera
    Qt3DRender::QCamera* cam = camera();
    cam->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    cam->setPosition(QVector3D(0, 0, -40.0f));
    cam->setUpVector(QVector3D(0, 1, 0));
    cam->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController* camController = new Qt3DExtras::QOrbitCameraController(rootEntity_);
    camController->setLinearSpeed(50.0f);
    camController->setLookSpeed(180.0f);
    camController->setCamera(cam);

    setRootEntity(rootEntity_);
}

// ----------------------------------------------------------------------------
SceneView::~SceneView()
{
}

// ----------------------------------------------------------------------------
void SceneView::addEntity(Qt3DCore::QEntity* entity)
{
    Qt3DRender::QMaterial* material = new Qt3DExtras::QPerVertexColorMaterial(entity);
    entity->addComponent(material);
    entity->setParent(rootEntity_);
}
