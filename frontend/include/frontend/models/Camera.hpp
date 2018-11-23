#pragma once

#include <QObject>
#include <QPoint>

class QMouseEvent;

namespace frontend {

class Camera : public QObject
{
    Q_OBJECT
public:
    enum Action
    {
        NONE,
        ROTATE,
        TRANSLATE,
        ZOOM
    };

    explicit Camera(QObject* parent=nullptr);
    ~Camera();

    void setSensitivity(Action action, float multiplier);

    void beginAction(Action action, QPoint mousePos);
    void endAction();

    void updateMouse(QPoint mousePos);

    void setCenter(float x, float y, float z);

    void getViewMatrix(float dst[16]);

private:

    QPoint lastMousePos_;

    float rotateSensitivity_;
    float translateSensitivity_;
    float zoomSensitivity_;

    float angleX_, angleY_;
    float x_, y_, z_;
    float distance_;

    Action currentAction_;
};

}
