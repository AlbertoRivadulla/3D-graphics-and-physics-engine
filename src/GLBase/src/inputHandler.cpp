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

GamepadInputHandler::GamepadInputHandler() { discoverGamepadID(); }

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

} // namespace GLBase
