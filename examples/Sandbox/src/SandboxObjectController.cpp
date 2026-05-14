#include "SandboxObjectController.h"

ObjectKeyboardInputHandler::ObjectKeyboardInputHandler(Physics::RigidBody *body) : mBody{body} {}

void ObjectKeyboardInputHandler::processInput(GLFWwindow *window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        mBody->addForceLocal(glm::vec3(0., 0., 10.));
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        mBody->addForceAtPointLocal(glm::vec3(0., 0., 3.), glm::vec3(0., 0.5, 0.));
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        mBody->addForce(glm::vec3(0., 0., 10.));
}

SandboxObjectController::SandboxObjectController(Entity *entity)
    : mKeyboardHandler(entity->getRigidBody()), mGamepadHandler(entity->getRigidBody()) {}

ObjectGamepadInputHandler::ObjectGamepadInputHandler(Physics::RigidBody *body) : mBody{body} {}

void ObjectGamepadInputHandler::processInput(float deltaTime) {
    if (mGamepadConnected) {
        GLFWgamepadstate state;

        if (glfwGetGamepadState(mGamepadID, &state)) {
            if (state.buttons[GLFW_GAMEPAD_BUTTON_CROSS]) {
                mBody->addForceLocal(glm::vec3(0., 0., 10.));
            }
            if (state.buttons[GLFW_GAMEPAD_BUTTON_CIRCLE]) {
                mBody->addForceAtPointLocal(glm::vec3(0., 0., 3.), glm::vec3(0., 0.5, 0.));
            }
            if (state.buttons[GLFW_GAMEPAD_BUTTON_SQUARE]) {
                mBody->addForce(glm::vec3(0., 0., 10.));
            }

            auto axesMoved = applyDeadZone(state.axes);
            if (axesMoved.rightTrigger) {
                mBody->addForceLocal(mRightTrigger * glm::vec3(0., 0., 10.));
            }
            if (axesMoved.leftTrigger) {
                mBody->addForceLocal(mLeftTrigger * glm::vec3(0., 0., -10.));
            }

            if (axesMoved.leftJoystick) {
                mBody->addTorqueLocal(mLeftX * glm::vec3(0., -1., 0.) + mLeftY * glm::vec3(-1., 0., 0.));
            }

        }
    }
}
