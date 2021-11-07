#ifndef SANDBOX_H
#define SANDBOX_H

#include "GLBase.h"
#include "GLGeometry.h"

using namespace GLGeometry;
using namespace GLBase;

class GLSandbox
{
    private:
        // Main application
        Application mApplication;

        // Main camera
        Camera mCamera;

        // Vector of GLObject instances
        std::vector<GLObject> mObjects;

        // Vector of model instances
        std::vector<Model> mModels;

        // Shaders
        std::vector<Shader> mShaders;
            
        // Value of the time elapsed since the last frame. This needs to be updated 
        // every frame
        float mDeltaTime;
        float mLastFrame;
        // Variables to measure the amount of time that each frame takes
        int mFrameCounter;
        float mTotalTime;
        
        // Setup the scene
        void setupScene();

        // Pass pointers to objects to the application, for the input processing
        void setupApplication();

        // Main render logic
        void render();

    public:
        // Constructor
        GLSandbox(int width, int height, const char* title);

        // Start the application's loop
        void run();
};

#endif
