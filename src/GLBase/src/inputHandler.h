#ifndef INPUTPROCESSOR_H
#define INPUTPROCESSOR_H

#include <GLFW/glfw3.h>
#include <vector>
#include "interfaces.h"

namespace GLBase {
class KeyboardInputHandler : public IValidatable {
public:
    virtual void processInput(GLFWwindow *window, float deltaTime) = 0;
};

class MouseInputHandler : public IValidatable {
public:
    virtual void processInput(double xpos, double ypos) = 0;
};

class ScrollInputHandler : public IValidatable {
public:
    virtual void processInput(double xoffset, double yoffset) = 0;
};

class GamepadInputHandler : public IValidatable {
public:
    virtual void processInput(float deltaTime) = 0;
};

class InputHandler {
public:
    void addKeyboardHandler(KeyboardInputHandler *handler);
    void addMouseHandler(MouseInputHandler *handler);
    void addScrollHandler(ScrollInputHandler *handler);
    void addGamepadHandler(GamepadInputHandler* handler);

    void processKeyboardInput(GLFWwindow *window, float deltaTime);
    void processMouseInput(double xpos, double ypos);
    void processScrollInput(double xoffset, double yoffset);
    void processGamepadInput(float deltaTime);

private:
    std::vector<KeyboardInputHandler *> mKeyboardHandlers;
    std::vector<MouseInputHandler *> mMouseHandlers;
    std::vector<ScrollInputHandler *> mScrollHandlers;
    std::vector<GamepadInputHandler*> mGamepadHandlers;
};
} // namespace GLBase

#endif
