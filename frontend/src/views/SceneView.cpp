#include "frontend/views/SceneView.hpp"
#include <QVBoxLayout>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QOrbitCameraController>

#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>

namespace frontend {

// ----------------------------------------------------------------------------
SceneView::SceneView(QScreen* screen) :
    Qt3DWindow(screen)
{
    Qt3DCore::QEntity* rootEntity = new Qt3DCore::QEntity;
    Qt3DRender::QMaterial* material = new Qt3DExtras::QPhongMaterial(rootEntity);

    Qt3DRender::QGeometryRenderer* customMeshRenderer = new Qt3DRender::QGeometryRenderer;
    Qt3DRender::QGeometry* customGeometry = new Qt3DRender::QGeometry(customMeshRenderer);

    QByteArray vertexArray;

//     Qt3DRender::QBuffer* vb = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, customGeometry);
//     Qt3DRender::QBuffer* ib = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, customGeometry);

    Qt3DCore::QEntity* torusEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QTorusMesh* torusMesh = new Qt3DExtras::QTorusMesh;
    torusMesh->setRadius(5);
    torusMesh->setMinorRadius(1);
    torusMesh->setRings(100);
    torusMesh->setSlices(20);

    Qt3DCore::QTransform* torusTransform = new Qt3DCore::QTransform;
    torusTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));

    torusEntity->addComponent(torusMesh);
    torusEntity->addComponent(torusTransform);
    torusEntity->addComponent(material);

    // Camera
    Qt3DRender::QCamera* cam = camera();
    cam->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cam->setPosition(QVector3D(0, 0, 40.0f));
    cam->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController* camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(cam);

    setRootEntity(rootEntity);
}

// ----------------------------------------------------------------------------
SceneView::~SceneView()
{
}

}
