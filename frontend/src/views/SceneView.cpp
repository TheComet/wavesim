#include "frontend/views/SceneView.hpp"
#include <QVBoxLayout>

using namespace frontend;

// ----------------------------------------------------------------------------
SceneView::SceneView(QScreen* screen) :
    Qt3DWindow(screen)
{
}

// ----------------------------------------------------------------------------
SceneView::~SceneView()
{
}
