#include "Camera.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"

Camera::Camera() {
    mEye = glm::vec3(0.0f, 0.0f, 0.0f);
    mViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    mUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(mEye, mEye+mViewDirection, mUpVector);
}

void Camera::MouseLook(int mouseX, int mouseY) {

    glm::vec2 currentMouse = glm::vec2(mouseX, mouseY);

    static bool firstLook = true;
    if (firstLook) {
        mOldMousePosition = currentMouse;
        firstLook = false;
    }

    glm::vec2 mouseDelta = mOldMousePosition - currentMouse;
    
    mViewDirection = glm::rotate(mViewDirection, glm::radians(mouseDelta.x), Camera::mUpVector);

    mOldMousePosition = currentMouse;
}

void Camera::MoveForward(float speed) {
    mEye += (mViewDirection*speed);
}

void Camera::MoveBackward(float speed) {
    mEye -= (mViewDirection*speed);
}

void Camera::MoveLeft(float speed) {
    glm::vec3 rightVector = glm::cross(mViewDirection, mUpVector);
    mViewDirection -= rightVector * speed;
}

void Camera::MoveRight(float speed) {
    glm::vec3 rightVector = glm::cross(mViewDirection, mUpVector);
    mViewDirection += rightVector * speed;
}