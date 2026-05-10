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

    void processInput(GLFWwindow *window, float deltaTime) override;

private:
    Physics::RigidBody *mBody;
};

class ObjectGamepadInputHandler : public GamepadInputHandler {
public:
    ObjectGamepadInputHandler() : mBody(nullptr) {}

    ObjectGamepadInputHandler(Physics::RigidBody *body);

    bool isValid() const override { return mBody != nullptr && mGamepadConnected; }

    void processInput(float deltaTime) override;

private:
    Physics::RigidBody *mBody;
};

class SandboxObjectController {
public:
    SandboxObjectController() = default;

    SandboxObjectController(Entity *body);

    ObjectKeyboardInputHandler *getKeyboardInputHandler() { return &mKeyboardHandler; }
    ObjectGamepadInputHandler *getGamepadInputHandler() { return &mGamepadHandler; }

private:
    ObjectKeyboardInputHandler mKeyboardHandler;
    ObjectGamepadInputHandler mGamepadHandler;
};

#endif
