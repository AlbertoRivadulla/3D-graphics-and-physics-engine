#ifndef SANDBOX_OBJECT_CONTROLLER_H
#define SANDBOX_OBJECT_CONTROLLER_H

#include "Entity.h"
#include "inputHandler.h"
#include "utils.h"

using namespace GLBase;

class ObjectKeyboardInputHandler : public KeyboardInputHandler {
public:
    ObjectKeyboardInputHandler() = default;

    ObjectKeyboardInputHandler(Physics::RigidBody *body);

    bool isValid() const override { return mBody != nullptr; }

    void processInput(GLFWwindow *window, float deltaTime) const override;

private:
    Physics::RigidBody *mBody;
};

class SandboxObjectController {
public:
    SandboxObjectController() = default;

    SandboxObjectController(Entity *body);

    ObjectKeyboardInputHandler *getKeyboardInputHandler() { return &mKeyboardHandler; }

private:
    ObjectKeyboardInputHandler mKeyboardHandler;
};

#endif
