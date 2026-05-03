#include "PlaneSim.h"

using namespace GLBase;
// using namespace GLGeometry;

PlaneSim::PlaneSim(int width, int height, const char *title, float scaling)
    : mApplication(width, height, title), mRenderer(),
      mLightingShader(mRenderer.getLightingShader()), // Reference to the G-pass
                                                      // shader of the renderer
      mCamera(width, height, glm::vec3(1., 0., 0.)), mAuxElements(width, height),
      mTextRenderer(width, height, std::string(BASE_DIR_RESOURCES) + "/fonts/Arial.ttf"), mGUIRenderer(width, height),
      mProjection{glm::mat4(1.)}, mView{glm::mat4(1.)}, mLastFrame{0.}, mFrameCounter{0}, mTotalTime{0.},
      mScrWidth{width}, mScrHeight{height} {
    // Get the actual resolution for the window.
    // This is needed in case we are using a "retina" display, where the
    // resolution is scaled.
    int winWidth;
    int winHeight;
    mApplication.getWindowDims(winWidth, winHeight);

    mRenderer.setupDimensions(winWidth, winHeight, width, height, scaling),

        Utils::seedRandomGeneratorClock();

    setupScene();

    setupApplication();
}

void PlaneSim::run() {
    while (!mApplication.mShouldClose) {
        mRenderer.startFrame();

        float currentFrame = glfwGetTime();
        mDeltaTime = currentFrame - mLastFrame;
        mLastFrame = currentFrame;

        mApplication.processKeyboardInput(mDeltaTime);

        float thisFrameTime{(float)glfwGetTime()};

        updateScene();

        mRenderer.computeShadowMaps(mCamera, mWorldManager.getGraphicsManager().getListOfLights(),
                                    mWorldManager.getGraphicsManager().getListOfObjects());

        mRenderer.startGeometryPass();

        renderDeferred();

        mRenderer.processGBuffer(mCamera.getPosition(), mWorldManager.getGraphicsManager().getListOfLights());

        mRenderer.endFrame(mWorldManager.getGraphicsManager().getSkymap());

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
