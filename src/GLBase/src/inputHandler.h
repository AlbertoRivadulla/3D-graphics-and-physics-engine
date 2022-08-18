#ifndef INPUTPROCESSOR_H
#define INPUTPROCESSOR_H

#include "GLBase.h"

namespace GLBase
{
    class KeyboardInputHandler
    {
        public:
            // Method to process input
            virtual void process(GLFWwindow* window, float deltaTime) const = 0;
    };

    class MouseInputHandler
    {
        public:
            // Method to process input
            virtual void process(double xpos, double ypos) const = 0;
    };

    class ScrollInputHandler
    {
        public:
            // Method to process input
            virtual void process(double xoffset, double yoffset) const = 0;
    };

    class ControllerInputHandler
    {
        public:
            // Method to process input
            virtual void process(GLFWwindow* window, float deltaTime) const = 0;
    };

    class InputHandler
    {
        public:
            // Methods to add input handlers
            void addKeyboardHandler(KeyboardInputHandler* handler);
            void addMouseHandler(MouseInputHandler* handler);
            void addScrollHandler(ScrollInputHandler* handler);
            // void addControllerHandler(ControllerInputHandler* handler);

            // Methods to process input
            void processKeyboardInput(GLFWwindow* window, float deltaTime);
            void processMouseInput(double xpos, double ypos);
            void processScrollInput(double xoffset, double yoffset);
            // void processControllerInput(float deltaTime);

        private:
            // Vectors of pointers to input handlers
            std::vector<KeyboardInputHandler*> mKeyboardHandlers;
            std::vector<MouseInputHandler*> mMouseHandlers;
            std::vector<ScrollInputHandler*> mScrollHandlers;
            // std::vector<ControllerInputHandler*> mScrollHanders;
    };
}

#endif
