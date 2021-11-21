#include "GLSandbox.h"

using namespace GLBase;
// using namespace GLGeometry;

// Constructor
GLSandbox::GLSandbox(int width, int height, const char* title) :
    mApplication(width, height, title),
    mLastFrame { 0. }, mFrameCounter { 0 }, mTotalTime { 0. },
    mProjection { glm::mat4(1.) }, mView { glm::mat4(1.) },
    mCamera(width, height, glm::vec3(0., 0., 0.)),
    mAuxElements(width, height),
    mRenderer(width, height, 1.f),
    mLightingShader ( mRenderer.getLightingShader() ) // Reference to the G-pass shader of the renderer
{
    // Seed a random number generator, with the function defined in utils.h
    GLUtils::seedRandomGeneratorClock();

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
        // Start the renderer
        // This also clears the window
        mRenderer.startFrame();

        // Compute the time since the last frame
        float currentFrame = glfwGetTime();
        mDeltaTime = currentFrame - mLastFrame;
        mLastFrame = currentFrame;

        // Process input for all the objects in the scene
        mApplication.processKeyboardInput(mDeltaTime);

        // Store in memory the time at the beginning of the drawing
        float thisFrameTime { (float)glfwGetTime() };

        // Update the scene
        updateScene();

        // Compute the shadow maps
        mRenderer.computeShadowMaps(mCamera, mLights, mElementaryObjects);

        // Start the geometry pass
        mRenderer.startGeometryPass();

        // Render the geometry that will use deferred rendering
        renderDeferred();

        // Do the shading pass
        mRenderer.processGBuffer(mCamera.Position, mLights);

        // Render the geometry that will use forward rendering
        renderForward();

        // End the renderer, which produces the final scene from the g-buffer
        mRenderer.endFrame();

        // Update the window, swapping the buffers
        mApplication.updateWindow();

        // Add the duration of this frame to the counter
        mTotalTime += (float)glfwGetTime() - thisFrameTime;
        // Add one to the counter
        ++mFrameCounter;
        // Every 60 frames, print the amount of time that each of them takes
        if (mFrameCounter % 60 == 0)
        {
            std::stringstream ss;
            // std::cout << "Average frame duration: " << mTotalTime * 1000 / mFrameCounter << " ms\n";
            ss << "Averate frame time: " << mTotalTime * 1000 / mFrameCounter << " ms - ";
            ss << "FPS: " << 1000. / (mTotalTime * 1000 / mFrameCounter);
            // Reset the variables
            mFrameCounter = 0;
            mTotalTime = 0;

            // Change the title of the application
            mApplication.setTitle(ss.str().c_str());
        }
    }

    std::cout << "Execution stopped\n";
}
