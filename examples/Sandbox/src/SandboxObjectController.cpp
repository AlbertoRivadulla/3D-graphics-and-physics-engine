#include "SandboxObjectController.h"
#include "utils.h"

ObjectKeyboardInputHandler::ObjectKeyboardInputHandler(Physics::RigidBody *body) : mBody{body} {
    LOG_INFO("Construct ObjectKeyboardInputHandler");
}

void ObjectKeyboardInputHandler::processInput(GLFWwindow *window, float deltaTime) const {

    // TODO:  Implement

    LOG_INFO("Process keyboard in ObjectKeyboardInputHandler");
}

SandboxObjectController::SandboxObjectController(Entity *entity) : mKeyboardHandler(entity->getRigidBody()) {}
