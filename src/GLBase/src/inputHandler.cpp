#include "GLBase.h"

namespace GLBase
{
    // Methods to add input handlers
    void InputHandler::addKeyboardHandler(KeyboardInputHandler* handler)
    {
        mKeyboardHandlers.push_back(handler);
    }

    void InputHandler::addMouseHandler(MouseInputHandler* handler)
    {
        mMouseHandlers.push_back(handler);
    }
    
    void InputHandler::addScrollHandler(ScrollInputHandler* handler)
    {
        mScrollHandlers.push_back(handler);
    }

    // void addControllerHandler(ControllerInputHandler* handler)
    // {
    //     mControllerHandlers.push_back(handler);
    // }

    // Methods to process input
    void InputHandler::processKeyboardInput(GLFWwindow* window, float deltaTime)
    {
        for (auto handler : mKeyboardHandlers)
        {
            handler->process(window, deltaTime);
        }
    }
    
    void InputHandler::processMouseInput(double xpos, double ypos)
    {
        for (auto handler : mMouseHandlers)
        {
            handler->process(xpos, ypos);
        }
    }

    void InputHandler::processScrollInput(double xoffset, double yoffset)
    {
        for (auto handler : mScrollHandlers)
        {
            handler->process(xoffset, yoffset);
        }
    }
    
    // void processControllerInput(float deltaTime)
    // {
    //     for (auto handler : mControllerHandlers)
    //     {
    //         handler->process(window, deltaTime);
    //     }
    // }
}
