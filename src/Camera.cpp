#include "Camera.hpp"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera() {
    mEye = glm::vec3(0.0f, 0.0f, 0.0f);
    mViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    mUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(mEye, mViewDirection, mUpVector);
}

void Camera::MoveForward(float speed) {
    mEye.z -= speed;
}

void Camera::MoveBackward(float speed) {
    mEye.z += speed;
}

void Camera::MoveLeft(float speed) {
    mViewDirection.x -= speed;
}

void Camera::MoveRight(float speed) {
    mViewDirection.x += speed;
}