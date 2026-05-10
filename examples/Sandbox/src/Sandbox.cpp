#include <memory>

#include "Sandbox.h"
#include "Colliders.h"
#include "ForceGenerator.h"
#include "Entity.h"
#include "RigidBody.h"
#include "SandboxObjectController.h"
#include "SandboxObserver.h"
#include "utils.h"

using namespace GLBase;
using namespace GLGeometry;
using namespace Utils;
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
void GLSandbox::setupScene() {
    // Initialize the GUI renderer
    mGUIRenderer.setGUISize(mScrWidth / 4, mScrHeight / 4);

    // // Create the cubemap for the sky
    // mGraphicsWorldManagerPtr->addSkymap<GLCubemap>();
    // mGraphicsWorldManagerPtr->getSkymap()->setupNoTextures(
    //     std::string(BASE_DIR_SHADERS) + "/GLGeometry/skyboxVertex.glsl",
    //     std::string(BASE_DIR_SHADERS) + "/GLGeometry/skyboxFragmentFlat.glsl");

    // Skymap with textures
    mGraphicsWorldManagerPtr->addSkymap<GLCubemap>();
    mGraphicsWorldManagerPtr->getSkymap()->setupWithTextures(std::string(BASE_DIR_RESOURCES) +
                                                                      "/textures/skybox");

    // Set the position of the camera
    // mCameraPtr->setPosition(glm::vec3(0.f, 0.f, 5.f));
    // mCameraPtr->setPosition(glm::vec3(0.f, 1.5f, 10.f));
    // mCameraPtr->setPosition(glm::vec3(0.f, 5.f, 20.f));
    mCameraPtr->setPosition(glm::vec3(20.f, -20.f, 30.f));

    // // Load a shader
    // mShaders.push_back(Shader(std::string(BASE_DIR_SHADERS) + "/vertex.glsl",
    // std::string(BASE_DIR_SHADERS) + "/fragment.glsl"));

    // Load shaders for the geometry pass
    mGPassShaders.push_back(Shader(std::string(BASE_DIR_SHADERS) + "/GLBase/defGeometryPassVertex.glsl",
                                   std::string(BASE_DIR_SHADERS) + "/GLBase/defGeometryPassFragment.glsl"));
    mGPassShaders.push_back(Shader(std::string(BASE_DIR_SHADERS) + "/GLBase/defGeometryPassVertexWithTextures.glsl",
                                   std::string(BASE_DIR_SHADERS) + "/GLBase/defGeometryPassFragmentWithTextures.glsl"));

    // Add a directional light
    mGraphicsWorldManagerPtr->addLight<DirectionalLight>(
        glm::vec3(1., 1., 1.),    // Color
        glm::vec3(10., 10., 10.), // Position
        glm::vec3(-1., -1., -1.), // Direction
        1.f, 0.f, 0.f             // Intensity, attenuation linear, attenuation quadratic
    );

    // Add a spotlight
    mGraphicsWorldManagerPtr->addLight<SpotLight>(glm::vec3(1., 0., 1.),                // Color
                                                           glm::vec3(3., 6., 10.),               // Position
                                                           glm::vec3(-1., -1., 0.),              // Direction
                                                           glm::radians(60.), glm::radians(90.), // Angles
                                                           7.,                                   // Intensity
                                                           0.05, 0.1 // Attenuation linear, attenuation quadratioc
    );

    // Add some point lights
    for (int i = 0; i < 2; ++i) {
        mGraphicsWorldManagerPtr->addLight<PointLight>(
            glm::vec3(getRandom0To1(), getRandom0To1(), getRandom0To1()), // Color
            glm::vec3(10.f * getRandom0To1() - 5.f, 10.f * getRandom0To1() - 2.f,
                      10.f * getRandom0To1() - 5.f), // Position
            2.f, 0.01f, 0.02f);
    }

    /*
       Add elements to the world
       -------------------------------------------------------------------------
    */

    // Setup force of gravity
    auto gravity = std::make_unique<GravityForceGenerator>(glm::vec3(0., -9.8, 0.));
    mWorldManager.getPhysicsManager().addGravity(std::move(gravity));

    // -------------------------------------------------------------------------
    // Terrain
    // -------------------------------------------------------------------------

    // Add a terrain
    // auto terrain = std::make_unique<Terrain>();
    // terrain->addPatchFromTexture(std::string(BASE_DIR_RESOURCES) + "/textures/heightmaps/iceland_heightmap.png", 0.25f,
    //                              0.4f, -15.f);
    // // terrain->addPatchFromTexture( std::string(BASE_DIR_RESOURCES) +
    // //     "/textures/heightmaps/heightmap-01.png", 0.25f, 0.4f, -15.f );
    // terrain->addMaterial<Material>(mGPassShaders[0], glm::vec3(0.5, 0.5, 0.2), 0.1);
    // // auto materialTerrain = std::make_unique<MaterialWithTextures>(mGPassShaders[1], glm::vec3(1., 0., 0.), 0.1);
    // // materialTerrain->loadAlbedoTexture(std::string(BASE_DIR_RESOURCES) + "/textures/wood.png");
    // // terrain->addMaterial(std::move(materialTerrain));
    // mWorldManager.addTerrain(std::move(terrain));

    // Add a terrain with to be drawn with the tesselation shader
    auto terrain = std::make_unique<Terrain>();
    terrain->addPatchFromTextureTessellated(
        std::string(BASE_DIR_RESOURCES) + "/textures/heightmaps/iceland_heightmap.png", 0.5f, 100.f, -80.f);
    // terrain->addPatchFromTextureTessellated(std::string(BASE_DIR_RESOURCES) + "/textures/heightmaps/heightmap-02.jpg",
    //                                         0.5f, 80.f, -80.f);
    terrain->addMaterial<Material>(terrain->getTessellationShader(), glm::vec3(0.5, 0.5, 0.2), 0.1);
    mGPassShaders.push_back(terrain->getTessellationShader());
    mWorldManager.addTerrain(std::move(terrain));

    // // Add a terrain to be drawn with the tesselation shader
    // int width = 200;
    // int height = 200;
    // float *heightData = new float[width * height];
    // for (int i = 0; i < width; i++) {
    //     for (int j = 0; j < height; j++) {
    //         float x = i - width / 2.f;
    //         float y = j - height / 2.f;
    //         heightData[j * width + i] = (150.f / (1.f + glm::sqrt(x * x + y * y)));
    //     }
    // }
    // auto terrain = std::make_unique<Terrain>();
    // terrain->addPatchFromHeightDataTessellated(heightData, width, height, 2.f, 20.f, -5.f);
    // delete[] heightData;
    // terrain->addMaterial<Material>(terrain->getTessellationShader(), glm::vec3(0.5, 0.5, 0.2), 0.1);
    // mGPassShaders.push_back(terrain->getTessellationShader());
    // mWorldManager.addTerrain(std::move(terrain));

    // -------------------------------------------------------------------------
    // Objects with physics
    // -------------------------------------------------------------------------

    // Add a sphere
    // The arguments of the constructor are position, scale, rotation angle,
    // rotation axis, mass, initial velocity
    auto sphere = std::make_unique<Entity>(glm::vec3(0., 5., 0.), glm::vec3(1., 1., 1.), 0.f, glm::vec3(1., 0., 0.));
    sphere->addGeometry<GLSphere>(16);
    sphere->addRigidBody<RigidBody>(1.f, InertiaTensors::sphere(1., 1.), glm::vec3(0., 0., 0.));
    sphere->addCollider<SphereCollider>();
    auto materialSphTextures = std::make_unique<MaterialWithTextures>(mGPassShaders[1], glm::vec3(1., 0., 0.), 0.1);
    materialSphTextures->loadAlbedoTexture(std::string(BASE_DIR_RESOURCES) + "/textures/world_8k.jpg");
    sphere->addMaterial(std::move(materialSphTextures));

    Entity *spherePtr = mWorldManager.addEntity(std::move(sphere));

    // Add gravity and a drag force to this object
    mWorldManager.getPhysicsManager().registerBodyGravity(spherePtr);
    auto dragForcePtr = mWorldManager.getPhysicsManager().addForce<DragForceGenerator>(0.9, 0.9, 0.9, 0.9);
    mWorldManager.getPhysicsManager().registerBodyForce(spherePtr, dragForcePtr);

    // Add a sphere
    // The arguments of the constructor are position, scale, rotation angle,
    // rotation axis, mass, initial velocity
    auto sphere2 = std::make_unique<Entity>(glm::vec3(0., 2., 0.), glm::vec3(1., 1., 1.), glm::degrees(45.f),
                                            glm::vec3(1., 0., 0.));
    sphere2->addGeometry<GLSphere>(16);
    sphere2->addRigidBody<RigidBody>(1.f, InertiaTensors::sphere(1., 1.), glm::vec3(0., 0., 0.));
    sphere2->addCollider<SphereCollider>();
    sphere2->addMaterial<Material>(mGPassShaders[0], glm::vec3(0., 1., 0.), 0.1);

    Entity *sphere2Ptr = mWorldManager.addEntity(std::move(sphere2));

    mWorldManager.getPhysicsManager().registerBodyGravity(sphere2Ptr);

    // Add a spring force between the two objects
    auto springForcePtr =
        mWorldManager.getPhysicsManager().addForce<SpringForceGenerator>(spherePtr->getRigidBody(), 5., 0.5, 0.5);
    mWorldManager.getPhysicsManager().registerBodyForce(sphere2Ptr, springForcePtr);

    // // Join it to the other sphere with a bungee
    // auto bungeeForcePtr =
    // mWorldManager.getPhysicsManager().addForce<BungeeForceGenerator>(spherePtr->getRigidBody(), 5., 10.);
    // mWorldManager.getPhysicsManager().registerBodyForce(sphere2Ptr, bungeeForcePtr);

    // Add a cylinder
    auto cylinder = std::make_unique<Entity>(glm::vec3(0., 5., 0.), glm::vec3(1., 1., 1.), glm::degrees(45.f),
                                             glm::vec3(1., 0., 0.));
    cylinder->addGeometry<GLCylinder>(16);
    cylinder->addRigidBody<RigidBody>(1., InertiaTensors::cylinder(1., 1., 1.), glm::vec3(0., 1., 0.));
    cylinder->addCollider<SphereCollider>();
    cylinder->addMaterial<Material>(mGPassShaders[0], glm::vec3(0., 0., 1.), 0.1);
    // Entity *cylinderPtr = mWorldManager.addEntity(std::move(cylinder));
    mWorldManager.addEntity(std::move(cylinder));

    // Add a cube
    auto cube = std::make_unique<Entity>(glm::vec3(0., 5., -1.), glm::vec3(1., 1., 1.), 0.f, glm::vec3(1., 0., 0.));
    cube->addGeometry<GLCube>();
    cube->addRigidBody<RigidBody>(1., InertiaTensors::box(1., 1., 1., 1.), glm::vec3(5., 0., 0.),
                                  glm::vec3(1.5, 1.5, 0.));
    cube->addCollider<ConvexCollider>(cube->getGeometry());
    cube->addMaterial<Material>(mGPassShaders[0], glm::vec3(1., 0., 0.), 0.1);
    mWorldManager.addEntity(std::move(cube));

    // Add a particle system
    auto particleSystem =
        std::make_unique<ParticleSystem>(mGPassShaders[0], glm::vec3(0., 1., 0.), glm::vec3(1., 1., 1.), 0.f,
                                         glm::vec3(1., 0., 0.), 1.f, glm::vec3(0., 0., 0.));
    particleSystem->setParticleGeometry(std::make_unique<GLSphere>(4), &mGPassShaders[0]);
    particleSystem->setParticleGravity({0.f, -5.f, 0.f});
    mWorldManager.addParticleSystem(std::move(particleSystem));

    // Add another cube that is controlled by the user
    auto controlledCube =
        std::make_unique<Entity>(glm::vec3(0., 5., 0.), glm::vec3(1., 1., 1.), 0.f, glm::vec3(1., 0., 0.));
    controlledCube->addGeometry<GLCube>();
    controlledCube->addRigidBody<RigidBody>(1., InertiaTensors::box(1., 1., 1., 1.), glm::vec3(0., 0., 0.),
                                            glm::vec3(0., 0., 0.));
    controlledCube->addCollider<ConvexCollider>(controlledCube->getGeometry());
    controlledCube->addMaterial<Material>(mGPassShaders[0], glm::vec3(1., 1., 0.), 0.1);
    auto controlledCubePtr = mWorldManager.addEntity(std::move(controlledCube));

    mObjectController = SandboxObjectController(controlledCubePtr);

    auto cubeDragForcePtr = mWorldManager.getPhysicsManager().addForce<DragForceGenerator>(0.4, 0.02, 0.1, 0.02);
    mWorldManager.getPhysicsManager().registerBodyForce(controlledCubePtr, cubeDragForcePtr);

    // // Add a test observer for this object
    // auto controlledCubeObs = mWorldManager.addObserver<PrintPositionEntityObserver>("Controlled cube");
    // controlledCubePtr->registerObserver(controlledCubeObs);

    // Add an observer that makes the camera follow the object
    auto cameraTrackCubeObs =
        mWorldManager.addObserver<CameraFollowEntityObserver>(mCameraPtr, controlledCubePtr->getPosition());
    controlledCubePtr->registerObserver(cameraTrackCubeObs);
}

// Pass pointers to objects to the application, for the input processing
// Also pass the pointer to the camera
void GLSandbox::setupApplication() {
    // Pass a pointer to the camera
    mApplication.setCamera(mCameraPtr);

    // // Set the camera to be orthographic
    // mCameraPtr->setOrthographic();

    // Pass a pointer to the input handler
    mApplication.setInputHandler(&mInputHandler);
    // Configure the frustum of the camera
    // mCameraPtr->setFrustum(0.1f, 200.f);
    mCameraPtr->setFrustum(0.1f, 500.f);

    // Pass pointers to the input handler of the camera
    mInputHandler.addKeyboardHandler(&mCameraKeyboardInputHandler);
    mInputHandler.addMouseHandler(&mCameraMouseInputHandler);
    mInputHandler.addScrollHandler(&mCameraScrollInputHandler);
    mInputHandler.addGamepadHandler(&mCameraGamepadInputHandler);

    // Pass to the input handler the controller for the object
    mInputHandler.addKeyboardHandler(mObjectController.getKeyboardInputHandler());

    // Pass the list of lights to the renderer, to configure the lighting shader
    mRenderer.configureLights(mGraphicsWorldManagerPtr->getListOfLights());
}

// Method to run on each frame, to update the scene
void GLSandbox::updateScene() {
    mWorldManager.simulationStep(mDeltaTime);

    mCameraPtr->update(mDeltaTime);

    // Get the view and projection matrices
    mProjection = mCameraPtr->getProjectionMatrix();
    mView = mCameraPtr->getViewMatrix();

    // Update the skymap
    mGraphicsWorldManagerPtr->getSkymap()->setViewProjection(mView, mProjection);
}

// Render the geometry that will use deferred rendering
void GLSandbox::renderDeferred() {
    // Configure the common uniforms in the shaders
    for (auto GPassShader : mGPassShaders) {
        GPassShader.use();
        GPassShader.setMat4("view", mView);
        GPassShader.setMat4("projection", mProjection);
    }

    // Draw all registered entities
    mWorldManager.draw();
}

// Render the geometry that will use forward rendering
void GLSandbox::renderForward() {
    // // Draw a point
    // mAuxElements.drawPoint(glm::vec3(-0., 0., -5.), mView, mProjection);
    // mAuxElements.drawPoint(glm::vec3(2., 1., -1.), mView, mProjection);
    // // Draw a line
    // mAuxElements.drawLine(glm::vec3(-1, 0., -5.), glm::vec3(2., 1., -1.),
    // mView, mProjection);
    // // Draw a parallelepiped
    // mAuxElements.drawParallelepiped(glm::vec3(-2., 0., 0.), glm::vec3(1., 0.,
    // 0.), glm::vec3(0., 1., 1.),
    //                            mView, mProjection);
    // // Draw a rectangle
    // mAuxElements.drawRectangle(glm::vec3(-2., 1., 0.), glm::degrees(-45.f), glm::vec3(1.,
    // 0., 0.), glm::vec3(1., 2., 1.),
    //                            mView, mProjection);
    // // Draw a box
    // mAuxElements.drawBox(glm::vec3(2., 1., 0.), glm::degrees(-45.f), glm::vec3(1., 0., 0.),
    // glm::vec3(1., 2., 1.),
    //                            mView, mProjection);
    // // Draw a cylinder
    // mAuxElements.drawCylinder(glm::vec3(-2., 3., 0.), glm::degrees(-45.f), glm::vec3(1., 0.,
    // 0.), glm::vec3(1., 2., 1.),
    //                            mView, mProjection);
    // // Draw a sphere
    // mAuxElements.drawSphere(glm::vec3(2., 0., -1.), 0., glm::vec3(1., 0.,
    // 0.), glm::vec3(1., 1., 1.),
    //                            mView, mProjection);
    // // Draw a cone
    // mAuxElements.drawCone(glm::vec3(-2., 0., 0.), 0., glm::vec3(1., 0., 0.),
    // glm::vec3(1., 1., 1.),
    //                            mView, mProjection);
    //
    // // Draw points in the positions of the lights
    // for (auto &light : mWorldManager.getGraphicsManager().getListOfLights())
    // {
    //     mAuxElements.drawPoint(light->getPosition(), mView, mProjection);
    // }

    //-------------------------------------------------------------------------
    // GUI and text

    // // Set the pixels of the GUI to zero
    // auto [guiWidth, guiHeight] = mGUIRenderer.getGUISize();
    // auto &guiPixels = mGUIRenderer.getPixels();
    // for (unsigned int i = 0; i < guiWidth * guiHeight * 4; ++i)
    //     guiPixels[i] = 0;
    // // Draw a red rectangle in the lowest corner
    // for (unsigned int x = 0; x < guiWidth; ++x) {
    //     for (unsigned int y = 0; y < guiHeight; ++y) {
    //         unsigned int index = (y * (guiWidth) + x) * 4;
    //         guiPixels[index + 0] = 255; // Red
    //         guiPixels[index + 1] = 0;   // Green
    //         guiPixels[index + 2] = 0;   // Blue
    //         guiPixels[index + 3] = 128; // Alpha (fully opaque)
    //     }
    // }
    //
    // // Draw the GUI elements
    // mGUIRenderer.pixelsToTexture(0, mScrHeight - guiHeight);
    // mGUIRenderer.renderGUI();

    // // Write text to the screen
    // mTextRenderer.renderText(std::to_string(mDeltaTime), 100.f, 100.f, 1.f, glm::vec3(0.f, 0.5f, 0.f));
    // mTextRenderer.renderText("This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.2f, 0.2f, 0.2f));
}
