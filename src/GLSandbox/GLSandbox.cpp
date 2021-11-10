#include "GLSandbox.h"

using namespace GLBase;
using namespace GLGeometry;

//==============================
// Methods of the GLSandbox class that depend on the scene to render
//==============================

// Setup the scene
//  - Create the camera
//  - Create/load geometry
//      - World
//      - Skymap
//      - Objects
//      - Gizmo
//  - Load shaders
//  - Create lights
void GLSandbox::setupScene()
{
    // // Create the cubemap for the sky
    // mSkymap = new GLCubemap();

    // Set the position of the camera
    mCamera.Position = glm::vec3(0.f, 0.f, 5.f);

    // Get teh projection matrix from the camera
    mProjection = mCamera.getPerspectiveProjection();
    // mProjection = mCamera.getOrthographicProjection();

    // Create a quad
    mElementaryObjects.push_back(new GLQuad());

    // Create a cube
    mElementaryObjects.push_back(new GLCube());

    // Load a shader
    mShaders.push_back(Shader("../shaders/vertex.glsl", "../shaders/fragment.glsl"));
    mShaders[0].use();
    mShaders[0].setVec3("lightPos", glm::vec3(1., 1., 2.));

    quadPosition = glm::vec3(0., 0., 0.);
}

// Pass pointers to objects to the application, for the input processing
// Also pass the pointer to the camera
void GLSandbox::setupApplication()
{
    // Pass a pointer to the camera
    mApplication.setCamera(&mCamera);
}

// Method to run on each frame, to update the scene
void GLSandbox::updateScene()
{
    // Get the view and projection matrices
    mProjection = mCamera.getPerspectiveProjection();
    // mProjection = mCamera.getOrthographicProjection();
    mView = mCamera.getViewMatrix();

    // // Update the skymap
    // mSkymap->setViewProjection(mView, mProjection);

    // Move the quad
    // mElementaryObjects[0]->setModelMatrix(glm::vec3(0., 0., -0.2), -90., glm::vec3(1.,0.,0.), glm::vec3(5.,5.,5.));
    mElementaryObjects[0]->setModelMatrix(glm::vec3(0., -1., 0.), -90., glm::vec3(1.,0.,0.), glm::vec3(5.,5.,5.));

    // Move the cube
    mElementaryObjects[1]->setModelMatrix(glm::vec3(2.,0.,0.), 0., glm::vec3(1.,0.,0.), glm::vec3(1.,1.,1.));
}

// Main render logic
void GLSandbox::render()
{
    // Configure the shader
    mShaders[0].use();
    mShaders[0].setMat4("view", mView);
    mShaders[0].setMat4("projection", mProjection);
    mShaders[0].setVec3("viewPos", mCamera.Position);

    // Draw the quad
    mShaders[0].setMat4("model", mElementaryObjects[0]->getModelMatrix());
    mShaders[0].setVec3("color", glm::vec3(1., 1., 0.));
    mElementaryObjects[0]->draw();

    // Draw the cube
    mShaders[0].setMat4("model", mElementaryObjects[1]->getModelMatrix());
    mShaders[0].setVec3("color", glm::vec3(1., 0., 0.));
    mElementaryObjects[1]->draw();

    // // Draw the skymap
    // mSkymap->draw();

    // Draw a gizmo
    mGizmo.draw(mView, mProjection);
}

