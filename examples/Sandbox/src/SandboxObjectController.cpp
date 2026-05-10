#include "SandboxObjectController.h"

ObjectKeyboardInputHandler::ObjectKeyboardInputHandler(Physics::RigidBody *body) : mBody{body} {}

void ObjectKeyboardInputHandler::processInput(GLFWwindow *window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        mBody->addForceLocal(glm::vec3(10., 0., 0.));
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        // mBody->addForceAtPointLocal(glm::vec3(5., 0., 0.), glm::vec3(0., 1., 0.));
        mBody->addForceAtPointLocal(glm::vec3(3., 0., 0.), glm::vec3(0., 0.5, 0.));
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        mBody->addForce(glm::vec3(10., 0., 0.));
}

SandboxObjectController::SandboxObjectController(Entity *entity) : mKeyboardHandler(entity->getRigidBody()) {}
