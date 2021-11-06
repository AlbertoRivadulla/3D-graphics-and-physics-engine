#ifndef WINDOW_H
#define WINDOW_H

#include "GLBase.h"

namespace GLBase
{
    class Application
    {
        public:
            // Constructor
            Application(int width, int height, const char* title, Camera* camera);
            // Destructor
            ~Application();

            // Set the callbacks as friend function
            friend void applicationFramebufferSizeCallback(GLFWwindow* window, int width, int height);
            friend void applicationMouseCallback(GLFWwindow* window, double xpos, double ypos);
            friend void applicationScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

            // Methods to get witdth and height
            int getWidth();
            int getHeight();

        private:
            // Pointer to a window object
            GLFWwindow* mWindow;

            // A pointer to a camera
            Camera* mCamera; 

            // Width and height of the window
            int mWidth;
            int mHeight;
            
            // Value of the time elapsed since the last frame. This needs to be updated 
            // every frame
            float mDeltaTime;
            float mLastFrame;
            // Variables to measure the amount of time that each frame takes
            int mFrameCounter;
            float mTotalTime;

            // Function to be called to process input that can modify the window, every frame
            void processInput();

            // Function to run every frame
            void render();

        public:
            // Function to start the application
            void run();
    };

    // Function to be called when the window is resized
    void applicationFramebufferSizeCallback(GLFWwindow* window, int width, int height);
    // Function to be called when the mouse is moved
    void applicationMouseCallback(GLFWwindow* window, double xpos, double ypos);
    // Function to be called on scroll
    void applicationScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
}

#endif
