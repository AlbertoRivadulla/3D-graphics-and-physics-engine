#include "Sandbox.h"
#include "ForceGenerator.h"

using namespace GLBase;
using namespace GLGeometry;
using namespace GLUtils;
using namespace Physics;

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
    // Create the pixels for the GUI, and initalize them all to zero
    mGUIWidth = mScrWidth / 4;
    mGUIHeight = mScrHeight / 4;
    mGUIPixels = new unsigned char[ mGUIWidth * mGUIHeight * 4 ];
    for ( unsigned int i = 0; i < mGUIWidth * mGUIHeight * 4; ++i )
        mGUIPixels[i] = 0;

    // Create the cubemap for the sky
    mSkymap = new GLCubemap();

    // // in GLCubemap.h and GLCubemap.cpp
    // // Skymap with textures. To use this, comment the constructor without textures
    // mSkymap = new GLCubemap( "../resources/textures/skybox" );

    // Set the position of the camera
    // mCamera.Position = glm::vec3(0.f, 0.f, 5.f);
    // mCamera.Position = glm::vec3(0.f, 1.5f, 10.f);
    mCamera.Position = glm::vec3(0.f, 1.5f, 30.f);

    // // Load a shader
    // mShaders.push_back(Shader(std::string(BASE_DIR_SHADERS) + "/vertex.glsl", std::string(BASE_DIR_SHADERS) + "/fragment.glsl"));

    // Load shaders for the geometry pass
    mGPassShaders.push_back(Shader(std::string(BASE_DIR_SHADERS) + "/GLBase/defGeometryPassVertex.glsl",
                                   std::string(BASE_DIR_SHADERS) + "/GLBase/defGeometryPassFragment.glsl"));
    mGPassShaders.push_back(Shader(std::string(BASE_DIR_SHADERS) + "/GLBase/defGeometryPassVertexWithTextures.glsl",
                                   std::string(BASE_DIR_SHADERS) + "/GLBase/defGeometryPassFragmentWithTextures.glsl"));

    // Add a directional light
    mLights.push_back(new DirectionalLight( {1., 1., 1.},     // Color
                                            {10., 10., 10.},  // Position
                                            {-1., -1., -1.},  // Direction
                                            1.f, 0.f, 0.f) ); // Intensity, attenuation linear, attenuation quadratic
    // // Add a spotlight
    // mLights.push_back(new SpotLight( {0., 1., 0.},            // Color
    //                                  {0., 4., 0.},            // Position
    //                                  {0., -1., 0.},           // Direction
    //                                  60.f, 90.f,              // Angles
    //                                  2.f, 0.05f, 0.1f) );     // Intensity, attenuation linear, attenuation quadratic
    // // Add some point lights
    // for (int i = 0; i < 2; ++i)
    // {
    //     mLights.push_back(new PointLight( {getRandom0To1(), getRandom0To1(), getRandom0To1()}, 
    //                                       {10.f * getRandom0To1() - 5.f, 10.f * getRandom0To1() - 2.f, 10.f * getRandom0To1() - 5.f}, 
    //                                       0.2f, 0.01f, 0.02f ) );
    // }

    /*
       Add elements to mPhysicsWorld
       -------------------------------------------------------------------------
       - Create the RigidBody or CollisionBody, as pointers (using "new ...")
       - Add it to mPhysicsWorld, either with .addRigidBody or .addCollisionBody
           - This also adds its GLObject to the list mElementaryObjects

       The objects are deleted from memory automatically, by the destructor of 
       mPhsyicsWorld
    */

    // Setup force of gravity
    mGravity = new Physics::GravityForceGenerator( { 0.f, -9.8f, 0.f } );

    // Add a plane
    // The arguments are position, scale, rotation angle and rotation axis
    CollisionBody* plane = new CollisionBody( { 0., -1., 0. },
                                              { 100., 100., 100. }, 
                                              -90., { 1., 0., 0. } );
    plane->addGeometry( new GLQuad(), mElementaryObjects );
    plane->addCollider( new PlaneCollider() );
    plane->addMaterial( new Material( mGPassShaders[0], { 0.5, 0.5, 0. }, 0.1 ) );
    mPhysicsWorld.addCollisionBody( plane );

    // Add a sphere
    // The arguments of the constructor are position, scale, rotation angle, 
    // rotation axis, mass, initial velocity
    RigidBody* sphere = new RigidBody( { 2., 0., -1. }, 
                                       { 1., 1., 1. },
                                       0.f, { 1., 0., 0. },
                                       1.f,
                                       { 10., 0., 0. } );
    sphere->addGeometry( new GLSphere(16), mElementaryObjects );
    sphere->addCollider( new SphereCollider() );
    // sphere->addMaterial( new Material( mGPassShaders[0], {1., 0., 0.}, 0.1 ) );
    MaterialWithTextures* materialSphTextures = new MaterialWithTextures( mGPassShaders[1], {1., 0., 0.}, 0.1 );
    materialSphTextures->loadAlbedoTexture( std::string(BASE_DIR_RESOURCES) + "/textures/world_8k.jpg" );
    sphere->addMaterial( materialSphTextures );
    mPhysicsWorld.addRigidBody( sphere );
    // Add a drag force to it
    mForces.push_back( new DragForceGenerator( 0.1, 0.1 ) );
    // mPhysicsWorld.addBodyForce( sphere, mForces[0] );
    mPhysicsWorld.addBodyForce( sphere, mForces.back() );


    // Add a sphere
    // The arguments of the constructor are position, scale, rotation angle, 
    // rotation axis, mass, initial velocity
    RigidBody* sphere2 = new RigidBody( { -25., 2., -1. }, 
                                        { 1., 1., 1. },
                                        45.f, { 1., 0., 0. },
                                        1.f,
                                        { 15., 15., 0. } );
    sphere2->addGeometry( new GLSphere(16), mElementaryObjects );
    sphere2->addCollider( new SphereCollider() );
    sphere2->addMaterial( new Material( mGPassShaders[0], {0., 1., 0.}, 0.1 ) );
    mPhysicsWorld.addRigidBody( sphere2 );
    // Add gravity to this object
    mPhysicsWorld.addBodyForce( sphere2, mGravity );

    // // Add a cylinder
    // // The arguments of the constructor are position, scale, rotation angle, 
    // // rotation axis, mass, initial velocity
    // RigidBody* cylinder = new RigidBody( { -5., 2., -1. }, 
    //                                     { 1., 1., 1. },
    //                                     45.f, { 1., 0., 0. },
    //                                     1.f,
    //                                     { 15., 15., 0. } );
    // cylinder->addGeometry( new GLCylinder(16), mElementaryObjects );
    // cylinder->addCollider( new SphereCollider() );
    // // cylinder->addMaterial( new Material( mGPassShaders[0], {0., 1., 0.}, 0.1 ) );
    // cylinder->addMaterial( materialSphTextures );
    // mPhysicsWorld.addRigidBody( cylinder );

    // // Add a cube
    // // The arguments of the constructor are position, scale, rotation angle, 
    // // rotation axis, mass, initial velocity
    // RigidBody* cube = new RigidBody( { 0., 0., -1. }, 
    //                                    { 1., 1., 1. },
    //                                    0.f, { 1., 0., 0. },
    //                                    1.f,
    //                                    { 0., 3., 0. } );
    // cube->addGeometry( new GLCube(), mElementaryObjects );
    // cube->addCollider( new ConvexCollider( new GLCube() ) );
    // cube->addMaterial( new Material( mGPassShaders[0], {0., 0., 1.}, 0.1 ) );
    // mPhysicsWorld.addRigidBody( cube );

    // // Add a particle system
    // ParticleSystem* particleSystem = new ParticleSystem( mGPassShaders[0],
    //                                                      { 0., 1., 0. },
    //                                                      { 1., 1., 1. },
    //                                                      0.f, { 1., 0., 0. },
    //                                                      1.f,
    //                                                      { 0., 0., 0. } );
    // particleSystem->setParticleGeometry( new GLSphere(4), mElementaryObjects, &mGPassShaders[0] );
    // particleSystem->setParticleGravity( { 0.f, -5.f, 0.f } );
    // mPhysicsWorld.addParticleSystem( particleSystem );
}

// Pass pointers to objects to the application, for the input processing
// Also pass the pointer to the camera
void GLSandbox::setupApplication()
{
    // Pass a pointer to the camera
    mApplication.setCamera(&mCamera);

    // // Set the camera to be orthographic
    // mCamera.setOrthographic();

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
    // Update the objects in the physics world
    mPhysicsWorld.step( mDeltaTime );

    // Get the view and projection matrices
    mProjection = mCamera.getProjectionMatrix();
    mView = mCamera.getViewMatrix();

    // Update the skymap
    mSkymap->setViewProjection(mView, mProjection);
}

// Render the geometry that will use deferred rendering
void GLSandbox::renderDeferred()
{
    // Configure the common uniforms in the shaders
    for ( auto GPassShader : mGPassShaders )
    {
        GPassShader.use();
        GPassShader.setMat4("view", mView);
        GPassShader.setMat4("projection", mProjection);
    }

    // Draw all the objects with physics
    // mPhysicsWorld.draw(mGPassShaders[0]);
    mPhysicsWorld.draw();
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
    // mAuxElements.drawSphere(glm::vec3(2., 0., -1.), 0., glm::vec3(1., 0., 0.), glm::vec3(1., 1., 1.),
    //                            mView, mProjection);
    // // Draw a cone
    // mAuxElements.drawCone(glm::vec3(-2., 0., 0.), 0., glm::vec3(1., 0., 0.), glm::vec3(1., 1., 1.),
    //                            mView, mProjection);

    // // Draw points in the positions of the lights
    // for (auto light : mLights)
    // {
    //     mAuxElements.drawPoint(light->getPosition(), mView, mProjection);
    // }


    //-------------------------------------------------------------------------
    // GUI and text

    // // Set the pixels of the GUI to zero
    // for ( unsigned int i = 0; i < mGUIWidth * mGUIHeight * 4; ++i )
    //     mGUIPixels[i] = 0;
    // // Draw a red rectangle in the lowest corner
    // for ( unsigned int x = 0; x < mGUIWidth; ++x )
    // {
    //     for ( unsigned int y = 0; y < mGUIHeight; ++y )
    //     {
    //         unsigned int index = ( y * ( mGUIWidth ) + x ) * 4;
    //         mGUIPixels[index + 0] = 255; // Red
    //         mGUIPixels[index + 1] = 0;   // Green
    //         mGUIPixels[index + 2] = 0;   // Blue
    //         mGUIPixels[index + 3] = 128; // Alpha (fully opaque)
    //     }
    // }
    //
    // // Draw the GUI elements
    // mGUIRenderer.pixelsToTexture( mGUIPixels, mGUIWidth, mGUIHeight, 0, mScrHeight - mGUIHeight );
    // mGUIRenderer.renderGUI();

    // Write text to the screen
    // mTextRenderer.renderText( std::to_string(mDeltaTime), 100.f, 100.f, 1.f, glm::vec3( 0.f, 0.5f, 0.f ) );
    // mTextRenderer.renderText( "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.2f, 0.2f, 0.2f));
}
