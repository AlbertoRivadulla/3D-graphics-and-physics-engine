#include "GLSandbox.h"

using namespace GLBase;
// using namespace GLGeometry;

// Constructor
GLSandbox::GLSandbox(int width, int height, const char* title) :
    mCamera(glm::vec3(0.f, 0.f, 5.f)), 
    mApplication(width, height, title)
{
    // Setup the scene
    setupScene();

    // Setup the pointers in the application
    setupApplication();
}

// Start the application's loop
void GLSandbox::run()
{
    while(!mApplication.mShouldClose)
    {
        // Compute the time since the last frame
        float currentFrame = glfwGetTime();
        mDeltaTime = currentFrame - mLastFrame;
        mLastFrame = currentFrame;

        // Process input for all the objects in the scene
        mApplication.processInput(mDeltaTime);

        // Store in memory the time at the beginning of the drawing
        float thisFrameTime { (float)glfwGetTime() };

        // Clear the window
        mApplication.clearWindow();

        // Do the rendering and swap buffers
        // render(renderCallback);
        render();

        // Update the window
        mApplication.updateWindow();

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

    std::cout << "Execution stopped\n";

}
