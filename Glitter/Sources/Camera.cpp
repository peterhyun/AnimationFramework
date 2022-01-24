#include "Camera.h"

#include <glm/gtx/transform.hpp>

Camera::Camera() : _zoom(45.f)
{
    _position = glm::vec3(0.0f, 0.0f, 0.0f);
    _front = glm::vec3(0.0f, 0.0f, -1.0f);
    _up = glm::vec3(0.0f, 1.0f, 0.0f);
    _right = glm::vec3(1.0f, 0.0f, 0.0f);
    _worldFront = glm::vec3(0.0f, 0.0f, -1.0f);
    _worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    _worldRight = glm::vec3(1.0f, 0.0f, 0.0f);
    Orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

Camera::~Camera()
{
    std::cout << "~camera" << std::endl;
}
/*
void Camera::keyboardInput(CamControl control, float deltaTime)
{
    float velocity = CAM_SPEED * deltaTime;

    switch(control)
    {
    case CamControl::FORWARD:
        _position += _front * velocity;
        break;
    case CamControl::BACKWARD:
        _position -= _front * velocity;
        break;
    case CamControl::LEFT:
        _position -= _right * velocity;
        break;
    case CamControl::RIGHT:
        _position += _right * velocity;
        break;
    default:
        std::cout << "unknown input " << control << std::endl;
        break;
    }
}
*/

void Camera::updateCameraVectors() {
    _front = Orientation * _worldFront;
    _right = Orientation * _worldRight;
    _up = Orientation * _worldUp;
}

void Camera::mouseInput(float xoffset, float yoffset)
{
    //std::cout << xoffset << ", " << yoffset << std::endl;
    xoffset *= CAM_SENSITIVITY;
    yoffset *= CAM_SENSITIVITY;
    Orientation = glm::angleAxis(glm::radians(-xoffset), _worldUp) * Orientation;
    Orientation = Orientation * glm::angleAxis(glm::radians(yoffset), _worldRight);
    updateCameraVectors();
}

void Camera::translate(float xoffset, float yoffset) {
    float velocity = CAM_SPEED * 0.02;
    xoffset *= CAM_SENSITIVITY;
    yoffset *= CAM_SENSITIVITY;
    _position -= _right * xoffset * velocity;
    _position -= _up * yoffset * velocity;
}

void Camera::mouseScrollInput(float yoffset) {
    yoffset *= 3.0f;
    _position -= _front * yoffset;
}
