#ifndef WINDOW_H
#define WINDOW_H

#include "GLBase.h"

namespace GLBase
{
    class Application
    {
        public:
            // Boolean variable that becomes true when the window should close
            bool mShouldClose;

            // Constructor
            Application(int width, int height, const char* title);
            // Destructor
            ~Application();

            // Set the callbacks as friend function
            friend void applicationFramebufferSizeCallback(GLFWwindow* window, int width, int height);
            friend void applicationMouseCallback(GLFWwindow* window, double xpos, double ypos);
            friend void applicationScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

            // Methods to get witdth and height
            int getWidth();
            int getHeight();

            // Methods to pass pointers to the camera and to input handlers
            void setCamera(Camera* camera);

            // Method to pass a pointer to the input handler
            void setInputHandler(InputHandler* inputHandler);

            // Function to be called to process input that can modify the window, every frame
            void processKeyboardInput(float deltaTime);

            // Function to clear the window
            void clearWindow();
            // Function called to update the window
            void updateWindow();

        private:
            // Pointer to a window object
            GLFWwindow* mWindow;

            // A pointer to a camera
            Camera* mCamera; 

            // Pointer to the input handler
            InputHandler* mInputHandler;

            // Width and height of the window
            int mWidth;
            int mHeight;
    };

    // Function to be called when the window is resized
    void applicationFramebufferSizeCallback(GLFWwindow* window, int width, int height);
    // Function to be called when the mouse is moved
    void applicationMouseCallback(GLFWwindow* window, double xpos, double ypos);
    // Function to be called on scroll
    void applicationScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
}

#endif
