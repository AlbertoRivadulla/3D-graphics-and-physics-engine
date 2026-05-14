#include "PlaneSim.h"
#include "Colliders.h"
#include "ForceGenerator.h"
#include "Entity.h"
#include "RigidBody.h"
#include "glm/trigonometric.hpp"
#include "src/logger.h"
#include "utils.h"
#include <memory>
#include "PlaneObserver.h"

using namespace GLBase;
using namespace GLGeometry;
using namespace Utils;
using namespace Physics;

//==============================
// Methods of the PlaneSim class that depend on the scene to render
//==============================

// Setup the scene
//  - Create the camera
//  - Create/load geometry
//      - World
//      - Skymap
//      - Objects
//  - Load shaders
//  - Create lights
void PlaneSim::setupScene() {
    // Initialize the GUI renderer
    mGUIRenderer.setGUISize(mScrWidth / 4, mScrHeight / 4);

    // Create the cubemap for the sky
    float sunTheta = -0.2f; // Theta = 0 corresponds to the ecuador
    float sunPhi = -1.5f;
    mGraphicsWorldManagerPtr->addSkymap<GLCubemap>();
    mGraphicsWorldManagerPtr->getSkymap()->setupNoTextures(std::string(BASE_DIR_SHADERS) +
                                 "/GLGeometry/skyboxVertex.glsl",
                             std::string(BASE_DIR_SHADERS) +
                                 "/GLGeometry/skyboxFragmentAtmosphere.glsl");
    mGraphicsWorldManagerPtr->getSkymap()->setSunPosition(sunPhi, sunTheta);

    // Set the position of the camera
    mCameraPtr->setPosition(glm::vec3(20.f, -20.f, 30.f));

    // Load shaders for the geometry pass
    mGPassShaders.push_back(Shader(std::string(BASE_DIR_SHADERS) +
                                       "/GLBase/defGeometryPassVertex.glsl",
                                   std::string(BASE_DIR_SHADERS) +
                                       "/GLBase/defGeometryPassFragment.glsl"));
    mGPassShaders.push_back(
        Shader(std::string(BASE_DIR_SHADERS) +
                   "/GLBase/defGeometryPassVertexWithTextures.glsl",
               std::string(BASE_DIR_SHADERS) +
                   "/GLBase/defGeometryPassFragmentWithTextures.glsl"));

    // Add a directional light
    mGraphicsWorldManagerPtr->addLight<DirectionalLight>(
        glm::vec3(1., 1., 1.),    // Color
        glm::vec3(10., 10., 10.), // Position
        glm::vec3(-1., -1., -1.), // Direction
        1.f, 0.f, 0.f             // Intensity, attenuation linear, attenuation quadratic
    );

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

    // Add a terrain with to be drawn with the tesselation shader
    auto terrain = std::make_unique<Terrain>();
    terrain->addPatchFromTextureTessellated(std::string(BASE_DIR_RESOURCES) + "/textures/heightmaps/heightmap-02.jpg",
                                            0.5f, 80.f, -80.f);
    terrain->addMaterial<Material>(terrain->getTessellationShader(), glm::vec3(0.5, 0.5, 0.2), 0.1);
    mGPassShaders.push_back(terrain->getTessellationShader());
    mWorldManager.addTerrain(std::move(terrain));

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
    auto dragForcePtr = mWorldManager.getPhysicsManager().addForce<DragForceGenerator>(0.9, 0.9, 0., 0.);
    mWorldManager.getPhysicsManager().registerBodyForce(spherePtr, dragForcePtr);

    // Add a sphere
    // The arguments of the constructor are position, scale, rotation angle,
    // rotation axis, mass, initial velocity
    auto sphere2 = std::make_unique<Entity>(glm::vec3(0., 2., 0.), glm::vec3(1., 1., 1.), glm::radians(45.f), glm::vec3(1., 0., 0.));
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

    // Add an observer that makes the camera follow the object
    auto cameraTrackPlaneObs =
        mWorldManager.addObserver<CameraFollowPlaneObserver>(mCameraPtr, sphere2Ptr->getPosition());
    sphere2Ptr->registerObserver(cameraTrackPlaneObs);
}

void PlaneSim::setupApplication() {
    mApplication.setCamera(mCameraPtr);

    // mCameraPtr->setOrthographic();

    mApplication.setInputHandler(&mInputHandler);

    // mCameraPtr->setFrustum(0.1f, 200.f);
    mCameraPtr->setFrustum(0.1f, 500.f);

    mInputHandler.addKeyboardHandler(&mCameraKeyboardInputHandler);
    mInputHandler.addMouseHandler(&mCameraMouseInputHandler);
    mInputHandler.addScrollHandler(&mCameraScrollInputHandler);
    mInputHandler.addGamepadHandler(&mCameraGamepadInputHandler);

    // Pass the list of lights to the renderer, to configure the lighting shader
    mRenderer.configureLights(mGraphicsWorldManagerPtr->getListOfLights());
}

void PlaneSim::updateScene() {
    mWorldManager.simulationStep(mDeltaTime);

    mCameraPtr->update(mDeltaTime);

    // Get the view and projection matrices
    mProjection = mCameraPtr->getProjectionMatrix();
    mView = mCameraPtr->getViewMatrix();

    // Update the skymap
    mGraphicsWorldManagerPtr->getSkymap()->setViewProjection(mView, mProjection);
    mGraphicsWorldManagerPtr->getSkymap()->setCameraPosition(mCameraPtr->getPosition());
}

void PlaneSim::renderDeferred() {
    for (auto GPassShader : mGPassShaders) {
        GPassShader.use();
        GPassShader.setMat4("view", mView);
        GPassShader.setMat4("projection", mProjection);
    }

    // Draw all registered entities
    mWorldManager.draw();
}

void PlaneSim::renderForward() {
}
