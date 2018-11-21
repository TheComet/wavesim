#pragma once

#include <QWindow>

namespace frontend {
class SceneLoader;

class SceneView : public QWindow
{
    Q_OBJECT
public:
    explicit SceneView(QScreen* screen=nullptr);
    explicit SceneView(QWindow* parent);
    ~SceneView();

    void draw();

private:
    void initGraphics();

private:
#ifdef Q_OS_LINUX
    void* X11Display_;
#endif
};

}
