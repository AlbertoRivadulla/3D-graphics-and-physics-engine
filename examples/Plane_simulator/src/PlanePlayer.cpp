#include "PlanePlayer.h"

void PlanePlayer::buildAndRegisterInManager(Physics::WorldManager &worldManager, Shader &shaderRef) {

    // auto entity = std::make_unique<Entity>(glm::vec3(0., 5., 0.), glm::vec3(1., 1., 2.), 0.f, glm::vec3(1., 0., 0.));
    //
    // // Build the composed geometry object
    // // The components that will be moved later have to be stored in member variables of the class
    // auto geometryObject = std::make_unique<GLComposedObject>();
    // auto cube1 =
    //     geometryObject->addComponent<GLCube>(glm::vec3(-1., 0., 0.), glm::vec3(1., 2., 1.), 0.f, glm::vec3(0., 0., 1.));
    // auto cube2 =
    //     geometryObject->addComponent<GLCube>(glm::vec3(+1., 0., 0.), glm::vec3(1., 1., 2.), 45.f, glm::vec3(0., 0., 1.));
    //
    // entity->addGeometry(std::move(geometryObject));
    //
    // entity->addRigidBody<Physics::RigidBody>(1., Physics::InertiaTensors::box(1., 1., 1., 1.), glm::vec3(0., 0., 0.),
    //                                          glm::vec3(0., 0., 0.));
    // entity->addMaterial<Material>(shaderRef, glm::vec3(1., 1., 0.), 0.1);
    // mEntityPtr = worldManager.addEntity(std::move(entity));
    //
    // // mObjectController = SandboxObjectController(mEntity);
    //
    // worldManager.getPhysicsManager().registerBodyGravity(mEntityPtr);
    //
    // auto cubeDragForcePtr =
    //     worldManager.getPhysicsManager().addForce<Physics::DragForceGenerator>(0.1, 0.005, 0.1, 0.02);
    // worldManager.getPhysicsManager().registerBodyForce(mEntityPtr, cubeDragForcePtr);
    //
    // // Add an observer that makes the camera follow the object
    // auto cameraTrackPlaneObs = worldManager.addObserver<CameraFollowPlaneObserver>(
    //     worldManager.getGraphicsManager().getCamera(), mEntityPtr->getPosition());
    // mEntityPtr->registerObserver(cameraTrackPlaneObs);
    //
    // // TODO: Build the plane
    // //  - [x] Create the plane entity
    // //  - [ ] Add geometry to the entity (initialize a composed object)
    // //      - [ ] Add elements to this geometry
    // //  - [ ] Add rigidbody (built by the plane)
    // //      - [ ] Compute the inertia tensor
    // //  - [ ] Add materials
    // //  - [ ] Register the entity in the world manager
    // //  - [ ] Store the pointer to the entity in the PlaneSim class
    // //  - [ ] Register forces to the body
    // //      - [ ] Gravity
    // //      - [ ] Other forces
    // //  - [ ] Register the observer that makes the camera follow
    // //  - [ ] Register the plane controller
}
