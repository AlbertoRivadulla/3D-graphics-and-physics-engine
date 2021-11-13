#ifndef SANDBOX_H
#define SANDBOX_H

#include "GLBase.h"
#include "GLGeometry.h"

using namespace GLGeometry;
using namespace GLBase;

class GLSandbox
{
    //==============================
    // User defined variables and logic
    //==============================

    private:

        glm::vec3 quadPosition;

        // Cubemap for the sky
        GLCubemap* mSkymap;
        
    //==============================
    // Basic implementation of the class
    //==============================

    private:
        // Main application
        Application mApplication;

        // Main camera
        Camera mCamera;

        // Main input handler
        InputHandler mInputHandler;

        // Object used to draw auxiliary geometry
        GLAuxElements mAuxElements;
        //
        // // Object used to draw gizmos when needed
        // GLGizmo mGizmo;

        // Projection and view matrices
        glm::mat4 mProjection;
        glm::mat4 mView;

        // Vector of GLElemObject instances
        std::vector<GLElemObject*> mElementaryObjects;

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

        // Method to run on each frame, to update the scene
        void updateScene();

        // Main render logic
        void render();

    public:
        // Constructor
        GLSandbox(int width, int height, const char* title);

        // Start the application's loop
        void run();
};

#endif
