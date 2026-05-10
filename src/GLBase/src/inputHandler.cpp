#include "inputHandler.h"
#include "utils.h"
// #include "GLBase.h"

namespace GLBase {

void InputHandler::addKeyboardHandler(KeyboardInputHandler *handler) {
    if (!handler->isValid()) {
        LOG_ERROR("InputHandler::addKeyboardHandler: the given handler is not valid.");
        return;
    }

    mKeyboardHandlers.push_back(handler);
}

void InputHandler::addMouseHandler(MouseInputHandler *handler) {
    if (!handler->isValid()) {
        LOG_ERROR("InputHandler::addMouseHandler: the given handler is not valid.");
        return;
    }

    mMouseHandlers.push_back(handler);
}

void InputHandler::addScrollHandler(ScrollInputHandler *handler) {
    if (!handler->isValid()) {
        LOG_ERROR("InputHandler::addScrollHandler: the given handler is not valid.");
        return;
    }

    mScrollHandlers.push_back(handler);
}

void InputHandler::addGamepadHandler(GamepadInputHandler *handler) {
    if (!handler->isValid()) {
        LOG_ERROR("InputHandler::addGamepadHandler: the given handler is not valid.");
        return;
    }

    mGamepadHandlers.push_back(handler);
}

void InputHandler::processKeyboardInput(GLFWwindow *window, float deltaTime) {
    for (const auto &handler : mKeyboardHandlers) {
        handler->processInput(window, deltaTime);
    }
}

void InputHandler::processMouseInput(double xpos, double ypos) {
    for (const auto &handler : mMouseHandlers) {
        handler->processInput(xpos, ypos);
    }
}

void InputHandler::processScrollInput(double xoffset, double yoffset) {
    for (const auto &handler : mScrollHandlers) {
        handler->processInput(xoffset, yoffset);
    }
}

void InputHandler::processGamepadInput(float deltaTime) {
    for (const auto &handler : mGamepadHandlers) {
        handler->processInput(deltaTime);
    }
}

GamepadInputHandler::GamepadInputHandler() : mXSensitivity(1.f), mYSensitivity(1.f), mDeadZone(0.1f) {
    discoverGamepadID();
}

void GamepadInputHandler::setSensitivity(float xSensitivity, float ySensitivity) {
    mXSensitivity = xSensitivity;
    mYSensitivity = ySensitivity;
}

void GamepadInputHandler::discoverGamepadID() {
    for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; ++i) {
        if (glfwJoystickPresent(i)) {
            if (glfwJoystickIsGamepad(i)) {
                mGamepadID = i;
                mGamepadConnected = true;
                const char *name = glfwGetJoystickName(i);
                LOG_DEBUG("Joystick " << i << " connected: " << name);

                return;
            }
        }
    }
    mGamepadConnected = false;
}

GamepadInputHandler::AxesMoved GamepadInputHandler::applyDeadZone(float *axes) {
    float &leftXValue = axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    float &leftYValue = axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
    float &rightXValue = axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
    float &rightYValue = axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
    float leftTriggerValue = (axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] + 1.f) * 0.5f;
    float rightTriggerValue = (axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] + 1.f) * 0.5f;

    // Scale so it starts at zero after the dead zone
    float normFactor = 1.f / (1.f - mDeadZone);

    GamepadInputHandler::AxesMoved result;

    if (std::abs(leftXValue) < mDeadZone && std::abs(leftYValue) < mDeadZone) {
        result.leftJoystick = false;
    } else {
        mLeftX = (leftXValue - std::copysign(mDeadZone, leftXValue)) * normFactor;
        mLeftY = (leftYValue - std::copysign(mDeadZone, leftYValue)) * normFactor;
    }

    if (std::abs(rightXValue) < mDeadZone && std::abs(rightYValue) < mDeadZone) {
        result.rightJoystick = false;
    } else {
        mRightX = (rightXValue - std::copysign(mDeadZone, rightXValue)) * normFactor;
        mRightY = (rightYValue - std::copysign(mDeadZone, rightYValue)) * normFactor;
    }

    if (std::abs(leftTriggerValue) < mDeadZone) {
        result.leftTrigger = false;
    } else {
        mLeftTrigger = (leftTriggerValue - std::copysign(mDeadZone, leftTriggerValue)) * normFactor;
    }

    if (std::abs(rightTriggerValue) < mDeadZone) {
        result.rightTrigger = false;
    } else {
        mRightTrigger = (rightTriggerValue - std::copysign(mDeadZone, rightTriggerValue)) * normFactor;
    }

    return result;
}

} // namespace GLBase
