#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"

class Camera {
    public:
        // Default Constructor
        Camera();
        // The ultiamate view matrix we will produce and return
        glm::mat4 mViewMatrix;

        glm::mat4 GetViewMatrix() const;

        void MouseLook(int mouseX, int mouseY);
        void MoveForward(float);
        void MoveBackward(float);
        void MoveLeft(float);
        void MoveRight(float);
    private:
        glm::vec3 mEye;
        glm::vec3 mViewDirection;
        glm::vec3 mUpVector;
        
        glm::vec2 mOldMousePosition;
};

#endif