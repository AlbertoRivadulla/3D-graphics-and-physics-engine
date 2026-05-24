#ifndef PLANE_SIM_H
#define PLANE_SIM_H

#include "GLBase.h"
#include "GLGeometry.h"
#include "Physics.h"
#include "WorldManager.h"
#include "cameraDefaultInputHandlers.h"
#include "PlanePlayer.h"

using namespace GLGeometry;
using namespace GLBase;

class PlaneSim {
private:
    //==============================
    // User defined variables and logic
    //==============================
    PlanePlayer mPlanePlayer;

    //==============================
    // Basic implementation of the class
    //==============================

private:
    // Main application
    Application mApplication;

    // Renderer
    DeferredRenderer mRenderer;
    // Reference to the shader of the Lighting pass
    Shader &mLightingShader;
    // Shaders for the geometry pass
    std::vector<Shader> mGPassShaders;

    // Main camera
    Camera *mCameraPtr;
    CameraKeyboardInputHandler mCameraKeyboardInputHandler;
    CameraMouseInputHandler mCameraMouseInputHandler;
    CameraScrollInputHandler mCameraScrollInputHandler;
    CameraGamepadInputHandler mCameraGamepadInputHandler;

    // Main input handler
    InputHandler mInputHandler;

    // Object used to draw auxiliary geometry
    GLAuxElements mAuxElements;
    // Object used to draw text to the screen
    GLTextRenderer mTextRenderer;
    // Object used to draw a GUI to the screen
    GLGUIRenderer mGUIRenderer;

    // Projection and view matrices
    glm::mat4 mProjection;
    glm::mat4 mView;

    // Vector of model instances
    std::vector<Model> mModels;

    // Shaders
    std::vector<Shader> mShaders;

    // Materials
    std::vector<Material> mMaterials;

    // Class containing all the objects with collisions and/or dynamics
    Physics::WorldManager mWorldManager;
    // Pointer to the graphics manager within the world manager
    GraphicsWorldManager *mGraphicsWorldManagerPtr;

    // Value of the time elapsed since the last frame. This needs to be updated
    // every frame
    float mDeltaTime;
    float mLastFrame;
    // Variables to measure the amount of time that each frame takes
    int mFrameCounter;
    float mTotalTime;

    // Screen width and height
    int mScrWidth;
    int mScrHeight;

    // Setup the scene
    void setupScene();

    // Pass pointers to objects to the application, for the input processing
    void setupApplication();

    // Method to run on each frame, to update the scene
    void updateScene();

    // Render the geometry that will use deferred rendering
    void renderDeferred();

    // Render the geometry that will use forward rendering
    void renderForward();

public:
    PlaneSim(int width, int height, const char *title, float scaling = 1.f);

    // Start the application's loop
    void run();
};

#endif
