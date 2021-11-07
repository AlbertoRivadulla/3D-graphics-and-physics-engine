#include "GLSandbox.h"

using namespace GLBase;
// using namespace GLGeometry;

// Constructor
GLSandbox::GLSandbox(int width, int height, const char* title) :
    mApplication(width, height, title),
    mLastFrame { 0. }, mFrameCounter { 0 }, mTotalTime { 0. },
    mProjection { glm::mat4(1.) }, mView { glm::mat4(1.) }
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

        // Update the scene
        updateScene();

        // Clear the window
        mApplication.clearWindow();

        // Render the scene
        render();

        // Update the window, swapping the buffers
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
