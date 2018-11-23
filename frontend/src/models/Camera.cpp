#include "frontend/models/Camera.hpp"
#include <QMouseEvent>
#include <cmath>

namespace frontend {

static const float PI = std::atan(1) * 4;

// ----------------------------------------------------------------------------
Camera::Camera(QObject* parent) :
    QObject(parent),
    rotateSensitivity_(1.0f),
    translateSensitivity_(1.0f),
    zoomSensitivity_(1.0f),
    angleX_(0.0f),
    angleY_(0.0f),
    x_(0.0f),
    y_(0.0f),
    z_(0.0f),
    distance_(1.0f),
    currentAction_(NONE)
{
}

// ----------------------------------------------------------------------------
Camera::~Camera()
{
}

// ----------------------------------------------------------------------------
void Camera::setSensitivity(Action action, float multiplier)
{
    switch (action)
    {
        case NONE      : break;
        case ROTATE    : rotateSensitivity_    = multiplier; break;
        case TRANSLATE : translateSensitivity_ = multiplier; break;
        case ZOOM      : zoomSensitivity_ = multiplier;  break;
    }
}

// ----------------------------------------------------------------------------
void Camera::beginAction(Action action, QPoint mousePos)
{
    if (currentAction_ != NONE)
        return;
    currentAction_ = action;
    lastMousePos_ = mousePos;
}

// ----------------------------------------------------------------------------
void Camera::endAction()
{
    currentAction_ = NONE;
}

// ----------------------------------------------------------------------------
void Camera::updateMouse(QPoint mousePos)
{
    float dx = mousePos.x() - lastMousePos_.x();
    float dy = mousePos.y() - lastMousePos_.y();
    lastMousePos_ = mousePos;

    switch (currentAction_)
    {
        case NONE : break;

        case ROTATE : {
            angleY_ -= dx * rotateSensitivity_ * 0.002;
            angleX_ -= dy * rotateSensitivity_ * 0.002;

            while (angleX_ > 2*PI) angleX_ -= 2*PI;
            while (angleX_ < 0) angleX_ += 2*PI;
            while (angleY_ > 2*PI) angleY_ -= 2*PI;
            while (angleY_ < 0) angleY_ += 2*PI;
        } break;

        case TRANSLATE : {
        } break;

        case ZOOM : {
            distance_ += dy * zoomSensitivity_ * distance_ * 0.002;
        } break;
    }
}

// ----------------------------------------------------------------------------
void Camera::setCenter(float x, float y, float z)
{
    x_ = x;
    y_ = y;
    z_ = z;
}

// ----------------------------------------------------------------------------
void Camera::getViewMatrix(float m[16])
{
    float cosx = std::cos(angleX_);
    float cosy = std::cos(angleY_);
    float sinx = std::sin(angleX_);
    float siny = std::sin(angleY_);

    // combined XY rotation matrix
    m[0] = cosy;       m[4] = 0;         m[8]  = siny;       m[12] = 0;
    m[1] = sinx*siny;  m[5] = cosx;      m[9]  = -sinx*cosy; m[13] = 0;
    m[2] = -cosx*siny; m[6] = sinx;      m[10] = cosx*cosy;  m[14] = 0;
    m[3] = 0;          m[7] = 0;         m[11] = 0;          m[15] = 1;
/*
    // rotate a unit vector to get camera position. This is the 3rd column in
    // the matrix above.
    float tx = m[8]  * distance_;
    float ty = m[9]  * distance_;
    float tz = m[10] * distance_;*/

    m[14] = distance_;
}

}
