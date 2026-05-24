#include "PlaneSim.h"
#include "camera.h"
#include "cameraDefaultInputHandlers.h"

using namespace GLBase;
// using namespace GLGeometry;

PlaneSim::PlaneSim(int width, int height, const char *title, float scaling)
    : mPlanePlayer(), mApplication(width, height, title), mRenderer(),
      mLightingShader(mRenderer.getLightingShader()), // Reference to the G-pass shader of the renderer
      mCameraKeyboardInputHandler(), mCameraMouseInputHandler(), mCameraScrollInputHandler(),
      mCameraGamepadInputHandler(), mAuxElements(width, height),
      mTextRenderer(width, height, std::string(BASE_DIR_RESOURCES) + "/fonts/Arial.ttf"), mGUIRenderer(width, height),
      mProjection{glm::mat4(1.)}, mView{glm::mat4(1.)}, mLastFrame{0.}, mFrameCounter{0}, mTotalTime{0.},
      mScrWidth{width}, mScrHeight{height} {
    // Get the actual resolution for the window.
    // This is needed in case we are using a "retina" display, where the
    // resolution is scaled.
    int winWidth;
    int winHeight;
    mApplication.getWindowDims(winWidth, winHeight);

    mRenderer.setupDimensions(winWidth, winHeight, width, height, scaling);

    // Store a pointer to the graphics world manager
    mGraphicsWorldManagerPtr = &mWorldManager.getGraphicsManager();

    // mCameraPtr = mGraphicsWorldManagerPtr->setupCamera<GLBase::Camera>(width, height, glm::vec3(1., 0., 0.));
    mCameraPtr = mGraphicsWorldManagerPtr->setupCamera<GLBase::OrbitalCamera>(width, height, glm::vec3(1., 0., 0.));

    mCameraKeyboardInputHandler.setCamera(mCameraPtr);
    mCameraMouseInputHandler.setCamera(mCameraPtr);
    mCameraScrollInputHandler.setCamera(mCameraPtr);
    mCameraGamepadInputHandler.setCamera(mCameraPtr);
    mCameraGamepadInputHandler.setSensitivity(4.f, 2.f);

    Utils::seedRandomGeneratorClock();

    setupScene();

    setupApplication();

    mLastFrame = glfwGetTime();
}

void PlaneSim::run() {
    while (!mApplication.mShouldClose) {
        mRenderer.startFrame();

        float currentFrame = glfwGetTime();
        // Clamp delta time to avoid spikes, to a maximum of 50ms (20 fms)
        mDeltaTime = glm::clamp(currentFrame - mLastFrame, 0.0f, 0.05f);
        mLastFrame = currentFrame;

        mApplication.processKeyboardInput(mDeltaTime);
        mApplication.processGamepadInput(mDeltaTime);

        float thisFrameTime{(float)glfwGetTime()};

        updateScene();

        mRenderer.computeShadowMaps(*mCameraPtr, mGraphicsWorldManagerPtr->getListOfLights(),
                                    mGraphicsWorldManagerPtr->getListOfObjects());

        mRenderer.startGeometryPass();

        renderDeferred();

        mRenderer.processGBuffer(mCameraPtr->getPosition(), mGraphicsWorldManagerPtr->getListOfLights());

        mRenderer.endFrame(mGraphicsWorldManagerPtr->getSkymap());

        renderForward();

        mApplication.updateWindow();

        mTotalTime += (float)glfwGetTime() - thisFrameTime;
        ++mFrameCounter;

        if (mFrameCounter % 300 == 0) {
            std::stringstream ss;
            ss << "Average frame time: " << mTotalTime * 1000 / mFrameCounter << " ms - ";
            ss << "FPS: " << 1000. / (mTotalTime * 1000 / mFrameCounter);
            mFrameCounter = 0;
            mTotalTime = 0;

            // // Change the title of the application
            // mApplication.setTitle(ss.str().c_str());
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
