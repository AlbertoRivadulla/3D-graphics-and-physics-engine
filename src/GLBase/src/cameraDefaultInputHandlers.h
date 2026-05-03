#ifndef CAMERA_DEFAULT_INPUT_HANDLERS_H
#define CAMERA_DEFAULT_INPUT_HANDLERS_H

#include <vector>

#include "camera.h"
#include "inputHandler.h"

namespace GLBase {

class CameraKeyboardInputHandler : public KeyboardInputHandler {
public:
    CameraKeyboardInputHandler(Camera *camera);

    bool isValid() const override { return mCamera != nullptr; }

    void processInput(GLFWwindow *window, float deltaTime) override;

private:
    Camera *mCamera;

    float mMovementSpeed;
};

class CameraMouseInputHandler : public MouseInputHandler {
public:
    CameraMouseInputHandler(Camera *camera);

    bool isValid() const override { return mCamera != nullptr; }

    void processInput(double xpos, double ypos) override;

private:
    Camera *mCamera;

    float mMouseSensitivity;

    // Boolean variable that determines whether it is the first mouse mouvement
    // in the execution
    bool mFirstMouse;

    // Last position of the mouse
    float mLastX;
    float mLastY;
};

class CameraScrollInputHandler : public ScrollInputHandler {
public:
    CameraScrollInputHandler(Camera *camera);

    bool isValid() const override { return mCamera != nullptr; }

    void processInput(double xoffset, double yoffset) override;

private:
    Camera *mCamera;
};

} // namespace GLBase

#endif
