#include "Sandbox.h"

using namespace GLBase;
// using namespace GLGeometry;

// Constructor
GLSandbox::GLSandbox(int width, int height, const char* title, float scaling) :
    mApplication(width, height, title),
    mRenderer(),
    mLightingShader ( mRenderer.getLightingShader() ), // Reference to the G-pass shader of the renderer
    mCamera(width, height, glm::vec3(1., 0., 0.)),
    mAuxElements(width, height),
    mTextRenderer(width, height, std::string(BASE_DIR_RESOURCES) + "/fonts/Arial.ttf"),
    mGUIRenderer(width, height),
    mProjection { glm::mat4(1.) }, mView { glm::mat4(1.) },
    mPhysicsWorld(),
    mLastFrame { 0. }, mFrameCounter { 0 }, mTotalTime { 0. },
    mScrWidth { width }, mScrHeight { height }
{
    // Get the actual resolution for the window.
    // This is needed in case we are using a "retina" display, where the resolution
    // is scaled.
    int winWidth;
    int winHeight;
    mApplication.getWindowDims( winWidth, winHeight );

    // Setup the renderer with the actual resolution for the window
    // The last argument is the scaling
    mRenderer.setupDimensions(winWidth, winHeight, width, height, scaling),

    // Seed a random number generator, with the function defined in utils.h
    Utils::seedRandomGeneratorClock();

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

        // End the renderer, which produces the final scene from the g-buffer
        mRenderer.endFrame(mSkymap);
        // mRenderer.endFrame(mSkymap, mAuxElements);

        // Render the geometry that uses forward rendering, after the deferred
        // renderer is done
        renderForward();

        // Update the window, swapping the buffers
        mApplication.updateWindow();

        // Add the duration of this frame to the counter
        mTotalTime += (float)glfwGetTime() - thisFrameTime;
        // Add one to the counter
        ++mFrameCounter;
        // Every 60 frames, print the amount of time that each of them takes
        // if (mFrameCounter % 60 == 0)
        if (mFrameCounter % 300 == 0)
        {
            std::stringstream ss;
            ss << "Average frame time: " << mTotalTime * 1000 / mFrameCounter << " ms - ";
            ss << "FPS: " << 1000. / (mTotalTime * 1000 / mFrameCounter);
            // Reset the variables
            mFrameCounter = 0;
            mTotalTime = 0;

            // // Change the title of the application
            // mApplication.setTitle(ss.str().c_str());

            // Print the frametime to the console
            LOG_INFO(ss.str());
        }

        // // Wait for the user to press a key
        // do 
        // {
        //     LOG_INFO('\n' << "Press a key to continue...");
        // } while (std::cin.get() != '\n');
    }

    LOG_INFO("Execution stopped");
}

// Destructor
GLSandbox::~GLSandbox()
{
    // Delete the skymap
    delete mSkymap;

    // Delete the elementary objects
    for ( auto object : mElementaryObjects )
        delete object;

    // Delete the lights
    for ( auto light : mLights ) 
        delete light;
}
