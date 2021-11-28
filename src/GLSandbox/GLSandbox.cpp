#include "GLSandbox.h"

using namespace GLBase;
using namespace GLGeometry;
using namespace GLUtils;

//==============================
// Methods of the GLSandbox class that depend on the scene to render
//==============================

// Setup the scene
//  - Create the camera
//  - Create/load geometry
//      - World
//      - Skymap
//      - Objects
//  - Load shaders
//  - Create lights
void GLSandbox::setupScene()
{
    // Create the cubemap for the sky
    mSkymap = new GLCubemap();
    // // Skymap with textures. To use this, comment the constructor without textures
    // // in GLCubemap.h and GLCubemap.cpp
    // mSkymap = new GLCubemap("../resources/textures/skybox");

    // Set the position of the camera
    mCamera.Position = glm::vec3(0.f, 0.f, 5.f);

    // Create a quad
    mElementaryObjects.push_back(new GLQuad());
    // Create a cube
    mElementaryObjects.push_back(new GLCube());
    // Create a cylinder
    mElementaryObjects.push_back(new GLCylinder(32));
    // Create a sphere
    mElementaryObjects.push_back(new GLSphere(16));
    // Create a cone
    mElementaryObjects.push_back(new GLCone(32));

    // Load a shader
    mShaders.push_back(Shader("../shaders/vertex.glsl", "../shaders/fragment.glsl"));

    // Load a shader for the geometry pass
    mGPassShaders.push_back(Shader("../shaders/GLBase/defGeometryPassVertex.glsl",
                                   "../shaders/GLBase/defGeometryPassFragment.glsl"));

    // Add some point lights
    for (int i = 0; i < 10; ++i)
    {
        mLights.push_back(new PointLight( {getRandom0To1(), getRandom0To1(), getRandom0To1()}, 
                                          {10.f * getRandom0To1() - 5.f, 10.f * getRandom0To1() - 2.f, 10.f * getRandom0To1() - 5.f}, 
                                          0.2f, 0.01f, 0.02f ) );
    }
    // Add a directional light
    mLights.push_back(new DirectionalLight( {1., 1., 1.},     // Color
                                            {10., 10., 10.},  // Position
                                            {-1., -1., -1.},  // Direction
                                            0.5f, 0.f, 0.f) ); // Intensity, attenuation linear, attenuation quadratic
    // Add a spotlight
    mLights.push_back(new SpotLight( {0., 1., 0.},            // Color
                                     {0., 4., 0.},            // Position
                                     {0., -1., 0.},           // Direction
                                     25.f, 90.f,              // Angles
                                     3.f, 0.05f, 0.1f) );    // Intensity, attenuation linear, attenuation quadratic

    // Create some materials
    mMaterials.push_back(Material( {1., 1., 0.}, 1.0 ));
    mMaterials.push_back(Material( {1., 0., 0.}, 1.0 ));
    mMaterials.push_back(Material( {0., 0., 1.}, 1.0 ));
    mMaterials.push_back(Material( {0., 1., 1.}, 1.0 ));
    mMaterials.push_back(Material( {0., 1., 0.}, 0.5 ));
}

// Pass pointers to objects to the application, for the input processing
// Also pass the pointer to the camera
void GLSandbox::setupApplication()
{
    // Pass a pointer to the camera
    mApplication.setCamera(&mCamera);
    // Pass a pointer to the input handler
    mApplication.setInputHandler(&mInputHandler);
    // // Configure the frustum of the camera
    // mCamera.setFrustum(0.1f, 50.f);

    // Pass pointers to the input handler of the camera
    mInputHandler.addKeyboardHandler(&mCamera.mKeyboardHandler);
    mInputHandler.addMouseHandler(&mCamera.mMouseHandler);
    mInputHandler.addScrollHandler(&mCamera.mScrollHandler);

    // Pass the list of lights to the renderer, to configure the lighting shader
    mRenderer.configureLights(mLights);
}

// Method to run on each frame, to update the scene
void GLSandbox::updateScene()
{
    // // Set the camera to be orthographic
    // mCamera.setOrthographic();

    // Get the view and projection matrices
    mProjection = mCamera.getProjectionMatrix();
    mView = mCamera.getViewMatrix();

    // Update the skymap
    mSkymap->setViewProjection(mView, mProjection);

    // Move the quad
    mElementaryObjects[0]->setModelMatrix(glm::vec3(0., -1., 0.), -90., glm::vec3(1.,0.,0.), glm::vec3(15.,15.,15.));

    // Move the cube
    mElementaryObjects[1]->setModelMatrix(glm::vec3(2.,0.,0.), 0., glm::vec3(1.,0.,0.), glm::vec3(2.,2.,2.));

    // Move the cylinder
    mElementaryObjects[2]->setModelMatrix({3.,1.,0.}, 0., glm::vec3(1.,0.,0.), glm::vec3(2.,2.,2.));

    // Move the sphere
    mElementaryObjects[3]->setModelMatrix(glm::vec3(-1.,0.,0.), 0., glm::vec3(1.,0.,0.), glm::vec3(2.,2.,2.));

    // Move the cone
    mElementaryObjects[4]->setModelMatrix(glm::vec3(-3.,0.,0.), 0., glm::vec3(1.,0.,0.), glm::vec3(2.,2.,2.));
}

// Render the geometry that will use deferred rendering
void GLSandbox::renderDeferred()
{
    // Configure the common uniforms in the shaders
    mGPassShaders[0].use();
    mGPassShaders[0].setMat4("view", mView);
    mGPassShaders[0].setMat4("projection", mProjection);

    // Draw the quad
    mGPassShaders[0].setMat4("model", mElementaryObjects[0]->getModelMatrix());
    mMaterials[0].configShader(mGPassShaders[0]);
    mElementaryObjects[0]->draw();

    // Draw the cube
    mGPassShaders[0].setMat4("model", mElementaryObjects[1]->getModelMatrix());
    mMaterials[1].configShader(mGPassShaders[0]);
    mElementaryObjects[1]->draw();

    // Draw the cylinder
    mGPassShaders[0].setMat4("model", mElementaryObjects[2]->getModelMatrix());
    mMaterials[2].configShader(mGPassShaders[0]);
    mElementaryObjects[2]->draw();

    // Draw the sphere
    mGPassShaders[0].setMat4("model", mElementaryObjects[3]->getModelMatrix());
    mMaterials[3].configShader(mGPassShaders[0]);
    mElementaryObjects[3]->draw();

    // Draw the cone
    mGPassShaders[0].setMat4("model", mElementaryObjects[4]->getModelMatrix());
    mMaterials[4].configShader(mGPassShaders[0]);
    mElementaryObjects[4]->draw();
}

// Render the geometry that will use forward rendering
void GLSandbox::renderForward()
{
    // // Draw a point
    // mAuxElements.drawPoint(glm::vec3(-0., 0., -5.), mView, mProjection);
    // mAuxElements.drawPoint(glm::vec3(2., 1., -1.), mView, mProjection);
    // // Draw a line
    // mAuxElements.drawLine(glm::vec3(-1, 0., -5.), glm::vec3(2., 1., -1.), mView, mProjection);
    // // Draw a parallelepiped
    // mAuxElements.drawParallelepiped(glm::vec3(-2., 0., 0.), glm::vec3(1., 0., 0.), glm::vec3(0., 1., 1.), 
    //                            mView, mProjection);
    // // Draw a rectangle
    // mAuxElements.drawRectangle(glm::vec3(-2., 1., 0.), -45., glm::vec3(1., 0., 0.), glm::vec3(1., 2., 1.),
    //                            mView, mProjection);
    // // Draw a box
    // mAuxElements.drawBox(glm::vec3(2., 1., 0.), -45., glm::vec3(1., 0., 0.), glm::vec3(1., 2., 1.),
    //                            mView, mProjection);
    // // Draw a cylinder
    // mAuxElements.drawCylinder(glm::vec3(-2., 3., 0.), -45., glm::vec3(1., 0., 0.), glm::vec3(1., 2., 1.),
    //                            mView, mProjection);
    // // Draw a sphere
    // mAuxElements.drawSphere(glm::vec3(-2., 1., 0.), 0., glm::vec3(1., 0., 0.), glm::vec3(1., 1., 1.),
    //                            mView, mProjection);
    // Draw a cone
    mAuxElements.drawCone(glm::vec3(-2., 0., 0.), 0., glm::vec3(1., 0., 0.), glm::vec3(1., 1., 1.),
                               mView, mProjection);

    // Draw points in the positions of the lights
    for (auto light : mLights)
    {
        mAuxElements.drawPoint(light->getPosition(), mView, mProjection);
    }
}
