#ifndef PLANE_PLAYER_H
#define PLANE_PLAYER_H

#include "WorldManager.h"
#include "PlaneObserver.h"
#include "AeroSurface.h"
#include "PlaneConfig.h"
#include <map>

struct ControlSurfaceState {
    glm::vec3 originalCenter;
    glm::mat4 originalModelMatrix;
    float rotationAngle;
    size_t graphicsObjIdx;
};

class PlanePlayer {
public:
    PlanePlayer() = default;

    void buildAndRegisterInManager(Physics::WorldManager &worldManager, Shader &shaderRef);

private:
    void setupPlaneGeometry(Entity *entity, Physics::WorldManager &worldManager, Shader &shaderRef);

    void setupControllerAndObservers(Physics::WorldManager &worldManager);

    void setupForces(Physics::WorldManager &worldManager);

    Entity *mEntityPtr;
    std::map<PlaneComponentIndex, ControlSurfaceState> mControlSurfaceStates;

    // TODO: Add members
    //  - Controller (for user input)
    //  - Force actuator (an instance of a class that implements the different forces the plane will experiment, besides gravity)
    //  - [?] Observer for the camera (shared pointer)
    //  - [?] Materials
};

#endif
