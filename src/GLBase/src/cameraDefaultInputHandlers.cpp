#include "cameraDefaultInputHandlers.h"

namespace GLBase {

// Keyboard input handler
//==============================

CameraKeyboardInputHandler::CameraKeyboardInputHandler() : GLBase::CameraKeyboardInputHandler(nullptr) {}

CameraKeyboardInputHandler::CameraKeyboardInputHandler(Camera *camera) : mCamera(camera), mMovementSpeed(SPEED) {}

void CameraKeyboardInputHandler::setCamera(Camera *camera) { mCamera = camera; }

void CameraKeyboardInputHandler::processInput(GLFWwindow *window, float deltaTime) {
    // The keys WASD move the camera around the scene
    float travelDistance{mMovementSpeed * deltaTime};
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        travelDistance *= 5.f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mCamera->moveFrontwards(travelDistance);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mCamera->moveFrontwards(-travelDistance);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mCamera->moveRightwards(-travelDistance);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mCamera->moveRightwards(travelDistance);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        mCamera->moveUpwards(travelDistance);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        mCamera->moveUpwards(-travelDistance);
}

// Mouse input handler
//==============================

CameraMouseInputHandler::CameraMouseInputHandler() : GLBase::CameraMouseInputHandler(nullptr) {}

CameraMouseInputHandler::CameraMouseInputHandler(Camera *camera)
    : mCamera(camera), mMouseSensitivity(MOUSE_SENSITIVITY), mFirstMouse(true), mLastX(0.f), mLastY(0.f) {}

void CameraMouseInputHandler::setCamera(Camera *camera) { mCamera = camera; }

void CameraMouseInputHandler::processInput(double xpos, double ypos) {
    // If it is the first time that the mouse is moved, the last position is
    // the same as the current one
    if (mFirstMouse) {
        mLastX = xpos;
        mLastY = ypos;
        mFirstMouse = false;
    }

    // Compute the change in the position of the mouse since the previous frame
    float xOffset = xpos - mLastX;
    float yOffset = mLastY - ypos; // reversed since y-coordinates go from bottom to top
    // Update the previous position of the mouse stored
    mLastX = xpos;
    mLastY = ypos;

    mCamera->moveMouseTargetAngles(xOffset * mMouseSensitivity, yOffset * mMouseSensitivity);
}

// Scroll input handler
//==============================

CameraScrollInputHandler::CameraScrollInputHandler() : GLBase::CameraScrollInputHandler(nullptr) {}

CameraScrollInputHandler::CameraScrollInputHandler(Camera *camera) : mCamera(camera) {}

void CameraScrollInputHandler::setCamera(Camera *camera) { mCamera = camera; }

void CameraScrollInputHandler::processInput(double xoffset, double yoffset) {
    mCamera->increaseDecreaseFov(-0.5 * (float)yoffset);
}

// Gamepad input handler
//==============================

CameraGamepadInputHandler::CameraGamepadInputHandler() : GLBase::CameraGamepadInputHandler(nullptr) {}

CameraGamepadInputHandler::CameraGamepadInputHandler(Camera *camera)
    : GamepadInputHandler(), mCamera(camera), mXSensitivity(1.f), mYSensitivity(1.f), mDeadZone(0.1f) {}

void CameraGamepadInputHandler::setCamera(Camera *camera) { mCamera = camera; }

void CameraGamepadInputHandler::setSensitivity(float xSensitivity, float ySensitivity) {
    mXSensitivity = xSensitivity;
    mYSensitivity = ySensitivity;
}

void CameraGamepadInputHandler::processInput(float deltaTime) {
    if (mGamepadConnected) {
        GLFWgamepadstate state;

        if (glfwGetGamepadState(mGamepadID, &state)) {
            if (state.buttons[GLFW_GAMEPAD_BUTTON_CROSS]) {
                LOG_INFO("Pressed cross");
            }

            if (applyDeadZone(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]))
            {
                mCamera->moveMouseTargetAngles(mRightX * deltaTime * mXSensitivity,
                                               -mRightY * deltaTime * mYSensitivity);
            }

            // TODO: Implement other
        }
    }
}

bool CameraGamepadInputHandler::applyDeadZone(float rightXValue, float rightYValue) {
    if (std::abs(rightXValue) < mDeadZone && std::abs(rightYValue) < mDeadZone) {
        return false;
    }

    // Scale so it starts at zero after the dead zone
    float normFactor = 1.f / (1.f - mDeadZone);
    mRightX = (rightXValue - std::copysign(mDeadZone, rightXValue)) * normFactor;
    mRightY = (rightYValue - std::copysign(mDeadZone, rightYValue)) * normFactor;

    return true;
}

} // namespace GLBase
