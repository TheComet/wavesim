#pragma once

#include <Qt3DExtras/Qt3DWindow>

namespace frontend {
class Scene;

class SceneView : public Qt3DExtras::Qt3DWindow
{
    Q_OBJECT
public:
    explicit SceneView(QScreen* screen=nullptr);
    ~SceneView();
};

}
