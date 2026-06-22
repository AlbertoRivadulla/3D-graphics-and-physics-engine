#include "PlanePlayer.h"
#include "PlaneConfig.h"
#include "glm/trigonometric.hpp"
#include "src/logger.h"
#include <ranges>

void PlanePlayer::buildAndRegisterInManager(Physics::WorldManager &worldManager, Shader &shaderRef) {
    // Build a composed geometry object
    // The components that will be moved later have to be stored in member variables of the class
    auto entity = std::make_unique<Entity>(glm::vec3(0., 5., 0.), glm::vec3(1., 1., 1.), 0.f, glm::vec3(1., 0., 0.));
    setupPlaneGeometry(entity.get(), worldManager, shaderRef);
    mEntityPtr = worldManager.addEntity(std::move(entity));

    setupControllerAndObservers(worldManager);

    setupForces(worldManager);

    // FIX: Remove this
    // Test: Rotate one aileron
    auto it = mControlSurfaceStates.find(PlaneComponentIndex::StarboardAileronWing);
    if (it != mControlSurfaceStates.end()) {
        auto &thisControlSurfaceState = it->second;

        // Set the rotation angle (this would be done by the controller)
        // Positive angles point upwards
        thisControlSurfaceState.rotationAngle = glm::radians(30.);

        // Set the model matrix for the corresponiding sub-object
        mEntityPtr->getGraphicsObject()->setModelMatrixAt(
            thisControlSurfaceState.graphicsObjIdx,
            Utils::rotateAroundPoint(thisControlSurfaceState.originalModelMatrix, thisControlSurfaceState.rotationAngle,
                                     glm::vec3(1., 0., 0.),
                                     glm::vec3(0., 0., AileronSize / 2.) + thisControlSurfaceState.originalCenter));
    } else {
        LOG_ERROR("Key " << static_cast<int>(PlaneComponentIndex::StarboardAileronWing) << " not found");
    }

    it = mControlSurfaceStates.find(PlaneComponentIndex::PortFlapWing);
    if (it != mControlSurfaceStates.end()) {
        auto &thisControlSurfaceState = it->second;

        // Set the rotation angle (this would be done by the controller)
        // Positive angles point upwards
        thisControlSurfaceState.rotationAngle = glm::radians(-30.);

        // Set the model matrix for the corresponiding sub-object
        mEntityPtr->getGraphicsObject()->setModelMatrixAt(
            thisControlSurfaceState.graphicsObjIdx,
            Utils::rotateAroundPoint(thisControlSurfaceState.originalModelMatrix, thisControlSurfaceState.rotationAngle,
                                     glm::vec3(1., 0., 0.),
                                     glm::vec3(0., 0., AileronSize / 2.) + thisControlSurfaceState.originalCenter));
    } else {
        LOG_ERROR("Key " << static_cast<int>(PlaneComponentIndex::StarboardAileronWing) << " not found");
    }
}

void PlanePlayer::setupPlaneGeometry(Entity *entity, Physics::WorldManager &worldManager, Shader &shaderRef) {
    // The local geometric origin is located at the center of the body (it is decoupled from the center of mass)

    auto *cubeGeometryPtr = worldManager.getGraphicsManager().addGeometryObject<GLCube>();
    auto *portWingsMaterial =
        worldManager.getGraphicsManager().addMaterial<Material>(shaderRef, glm::vec3(1., 1., 0.), 0.1);
    auto *starboardWingsMaterial =
        worldManager.getGraphicsManager().addMaterial<Material>(shaderRef, glm::vec3(1., 0., 1.), 0.1);
    auto *fuselageMaterial =
        worldManager.getGraphicsManager().addMaterial<Material>(shaderRef, glm::vec3(0., 1., 1.), 0.1);
    auto *aileronsMaterial =
        worldManager.getGraphicsManager().addMaterial<Material>(shaderRef, glm::vec3(1., 0., 0.), 0.1);
    auto *flapsMaterial =
        worldManager.getGraphicsManager().addMaterial<Material>(shaderRef, glm::vec3(0., 1., 0.), 0.1);

    auto *graphicsObject = entity->getGraphicsObject();

    // Add the first object (the fuselage) separately
    const auto &fuselageConf = PlaneElementConfigs[static_cast<size_t>(PlaneComponentIndex::Fuselage)];
    entity->addObject(cubeGeometryPtr, fuselageMaterial);
    graphicsObject->setModelMatrixAt(static_cast<size_t>(PlaneComponentIndex::Fuselage),
                                     fuselageConf.relativePos - PlaneCenterOffset, fuselageConf.relativeRotationAngle,
                                     fuselageConf.relativeRotationAxis, fuselageConf.scale);
    entity->addRigidBody<Physics::RigidBody>(fuselageConf.mass, fuselageConf.localInertiaTensor,
                                             fuselageConf.relativePos + fuselageConf.centerOfMassWrtRelativePos,
                                             glm::vec3(0., 0., 0.), glm::vec3(0., 0., 0.) // Initial velocities
    );

    // Iterate over the rest of the elements
    for (size_t i : std::views::iota(size_t(0), PlaneElementConfigs.size())) {
        if (i == static_cast<size_t>(PlaneComponentIndex::Fuselage)) {
            continue;
        }

        const auto &elementConf = PlaneElementConfigs[i];

        entity->addObject(cubeGeometryPtr, i % 2 == 0 ? starboardWingsMaterial : portWingsMaterial);
        graphicsObject->setModelMatrixAt(i, elementConf.relativePos - PlaneCenterOffset,
                                         elementConf.relativeRotationAngle, elementConf.relativeRotationAxis,
                                         elementConf.scale);
        entity->addRigidBodyComponent(i, elementConf.localInertiaTensor,
                                      elementConf.relativePos + elementConf.centerOfMassWrtRelativePos,
                                      elementConf.relativeRotationAngle, elementConf.relativeRotationAxis);
    }

    // Setup objects for ailerons (only for drawing)
    for (auto i : PlaneElemsWithAileronIndices) {
        const auto &elementConf = PlaneElementConfigs[static_cast<size_t>(i)];

        entity->addObject(cubeGeometryPtr, aileronsMaterial);
        size_t graphicsObjIdx = static_cast<size_t>(graphicsObject->getCountObjects()) - 1;

        glm::vec3 aileronPosition = elementConf.relativePos;
        aileronPosition.z -= (elementConf.scale.z + AileronSize) / 2.;
        glm::vec3 aileronScale = elementConf.scale;
        aileronScale.z = AileronSize;

        graphicsObject->setModelMatrixAt(graphicsObjIdx, aileronPosition - PlaneCenterOffset,
                                         elementConf.relativeRotationAngle, elementConf.relativeRotationAxis,
                                         aileronScale);

        const auto modelMatrix = graphicsObject->getModelMatrixAt(graphicsObjIdx);
        mControlSurfaceStates.insert(std::make_pair(
            i, ControlSurfaceState{aileronPosition - PlaneCenterOffset, modelMatrix, 0.f, graphicsObjIdx}));
    }

    // Setup objects for flaps (only for drawing)
    for (auto i : PlaneElemsWithFlapIndices) {
        const auto &elementConf = PlaneElementConfigs[static_cast<size_t>(i)];

        entity->addObject(cubeGeometryPtr, flapsMaterial);
        size_t graphicsObjIdx = static_cast<size_t>(graphicsObject->getCountObjects()) - 1;

        glm::vec3 flapPosition = elementConf.relativePos;
        flapPosition.z -= (elementConf.scale.z + FlapSize) / 2.;
        glm::vec3 flapScale = elementConf.scale;
        flapScale.z = FlapSize;

        graphicsObject->setModelMatrixAt(graphicsObjIdx, flapPosition - PlaneCenterOffset,
                                         elementConf.relativeRotationAngle, elementConf.relativeRotationAxis,
                                         flapScale);

        const auto modelMatrix = graphicsObject->getModelMatrixAt(graphicsObjIdx);
        mControlSurfaceStates.insert(
            std::make_pair(i, ControlSurfaceState{flapPosition - PlaneCenterOffset, modelMatrix, 0.f, graphicsObjIdx}));
    }
}

void PlanePlayer::setupControllerAndObservers(Physics::WorldManager &worldManager) {
    // Add an observer that makes the camera follow the object
    auto cameraTrackPlaneObs = worldManager.addObserver<CameraFollowPlaneObserver>(
        worldManager.getGraphicsManager().getCamera(), mEntityPtr->getPosition());
    mEntityPtr->registerObserver(cameraTrackPlaneObs);

    // TODO:
    //  - [ ] Pass the aileron and flap model matrices to the object that keeps their state
    //  - [ ] Register the plane controller

    LOG_WARNING("Implement setupControllerAndObservers")

    // TODO: Do a validation step to check that all the control surfaces managed by the controller exist in the map
}

void PlanePlayer::setupForces(Physics::WorldManager &worldManager) {
    // TODO:
    //  - [ ] Register forces to the body
    //      - [ ] Gravity
    //      - [ ] Other forces

    LOG_WARNING("Implement setupForces")

    // worldManager.getPhysicsManager().registerBodyGravity(mEntityPtr);

    auto cubeDragForcePtr =
        worldManager.getPhysicsManager().addForce<Physics::DragForceGenerator>(0.1, 0.005, 0.1, 0.02);
    worldManager.getPhysicsManager().registerBodyForce(mEntityPtr, cubeDragForcePtr);
}
