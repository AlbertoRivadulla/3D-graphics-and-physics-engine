#include "GLBase.h"
// #include "application.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace GLBase
{
    //==============================
    // Methods of the Window class
    //==============================

    // Constructor
    Application::Application(int width, int height, const char* title, Camera* camera) :
        mWidth { width }, mHeight { height }, mCamera { camera },
        mLastFrame { 0. }, mFrameCounter { 0 }, mTotalTime { 0. }
    {
        // Initialize GLFW
        glfwInit();
        // Configure GLFW
        // The first option of glfwWindowHint is the option that we want to configure,
        // and the second argument is the value
        // List of configurations: https://www.glfw.org/docs/latest/window.html#window_hints
        // Tell GLFW to use OpenGL 3.3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        // Tell GLFW explicitly to use the core profile.
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // Needed for Mac Os.
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        // Create a Window object, and check that it was created successfully.
        // GLFWwindow* window { glfwCreateWindow(800, 600, "My title", NULL, NULL) };
        mWindow = glfwCreateWindow(width, height, title, NULL, NULL);
        if (mWindow == NULL)
        {
            std::cout << "Failed to create GLFW window.\n";
            glfwTerminate();
        }
        glfwMakeContextCurrent( mWindow );

        glfwSetWindowUserPointer(mWindow, this);

        // Initialize GLAD
        // This library manages function pointers for OpenGL.
        // glfwGetProcAddress defines the correct function to load the address of the 
        // OpenGL function pointers, based on which OS we're compiling for
        if (!gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress) )
        {
            std::cout << "Failed to initialize GLAD.\n";
        }

        // Tell OpenGL the size of the rendering window
        // The first two parameters are the location of the lower left corner of the window.
        glViewport(0, 0, width, height);

        // Tell GLFW to call this function on every window resize by registering it.
        glfwSetFramebufferSizeCallback(mWindow, applicationFramebufferSizeCallback);
        // Set callback functions for mouse movement and scroll
        glfwSetCursorPosCallback(mWindow, applicationMouseCallback);
        glfwSetScrollCallback(mWindow, applicationScrollCallback);
        // tell GLFW to capture our mouse
        glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Configure the global state of OpenGL
        // Set depth test (default)
        glEnable(GL_DEPTH_TEST);
        // Enable multisampling
        glEnable(GL_MULTISAMPLE);
        // Enable face culling
        glEnable(GL_CULL_FACE);
        // Enable gamma correction the easy way
        // glEnable(GL_FRAMEBUFFER_SRGB);
    }

    // Destructor
    Application::~Application()
    {
        // std::cout << "Closing application... ";
        glfwTerminate();
        // std::cout << "Application closed\n";
    }

    // Methods to get witdth and height
    int Application::getWidth()
    {
        return mWidth;
    }
    int Application::getHeight()
    {
        return mHeight;
    }

    // Process all input
    void Application::processInput()
    {
        // Process input for the window
        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mWindow, true);

        // Process input for all the objects in the frame
        mCamera->processKeyboardInput(mWindow, mDeltaTime);
    }

    // Function to update the application every frame
    void Application::render()
    {
        // render
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Application::run()
    {
        while(!glfwWindowShouldClose(mWindow))
        {
            // per-frame time logic
            // --------------------
            float currentFrame = glfwGetTime();
            mDeltaTime = currentFrame - mLastFrame;
            mLastFrame = currentFrame;

            // Process input for all the objects in the scene
            processInput();

            // Store in memory the time at the beginning of the drawing
            float thisFrameTime { (float)glfwGetTime() };

            // Do the rendering and swap buffers
            render();

            // Swap the buffers to present the image on the screen
            glfwSwapBuffers(mWindow);
            // Poll for IO events
            glfwPollEvents();

            // Add the duration of this frame to the counter
            mTotalTime += (float)glfwGetTime() - thisFrameTime;
            // Add one to the counter
            ++mFrameCounter;
            // Every 300 frames, print the amount of time that each of them takes
            if (mFrameCounter % 300 == 0)
            {
                std::cout << "Average frame duration: " << mTotalTime * 1000 / mFrameCounter << " ms\n";
                // Reset the variables
                mFrameCounter = 0;
                mTotalTime = 0;
            }
        }
    }


    //==============================
    // Callback function to act on the Window class
    //==============================

    // Function to be called when the window is resized
    void applicationFramebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }

    // Function to be called when the mouse is moved
    void applicationMouseCallback(GLFWwindow* window, double xpos, double ypos)
    {
        Application* app { static_cast<Application*>(glfwGetWindowUserPointer(window)) };
        // // if (app->mFirstMouse)
        // // {
        // //     app->mLastX = xpos;
        // //     app->mLastY = ypos;
        // //     app->mFirstMouse = false;
        // // }
        //
        // float xoffset = xpos - app->mLastX;
        // float yoffset = app->mLastY - ypos; // reversed since y-coordinates go from bottom to top
        //
        // app->mLastX = xpos;
        // app->mLastY = ypos;
        //
        // app->mCamera->processMouseInput(xoffset, yoffset);

        app->mCamera->processMouseInput(xpos, ypos);
    }

    // Function to be called on scroll
    void applicationScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        Application* app { static_cast<Application*>(glfwGetWindowUserPointer(window)) };
        app->mCamera->processScrollInput(xoffset, yoffset);
    }
}
