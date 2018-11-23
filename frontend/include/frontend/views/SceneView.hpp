#pragma once

#include <QWindow>

namespace frontend {
class SceneLoader;
class Camera;

class SceneView : public QWindow
{
    Q_OBJECT
public:
    explicit SceneView(QScreen* screen=nullptr);
    explicit SceneView(QWindow* parent);
    ~SceneView();

protected:
    void resizeEvent(QResizeEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

private slots:
    void onResizeTimerTimeout();

private:
    void initGraphics();
    void draw();

private:
    QTimer* resizeTimer_;
    Camera* camera_;
};

}
