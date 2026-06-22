#ifndef PLANE_PLAYER_H
#define PLANE_PLAYER_H

#include "WorldManager.h"
#include "PlaneObserver.h"
#include "AeroSurface.h"
#include "PlaneConfig.h"

class PlanePlayer {
public:
    PlanePlayer() = default;

    void buildAndRegisterInManager(Physics::WorldManager &worldManager, Shader &shaderRef);

private:
    Entity *mEntityPtr;

    void setupPlaneGeometry(Entity *entity, Physics::WorldManager &worldManager, Shader &shaderRef);

    void setupForces(Physics::WorldManager &worldManager);

    void setupControllerAndObservers(Physics::WorldManager &worldManager);

    // TODO: Add members
    //  - Controller (for user input)
    //  - Force actuator (an instance of a class that implements the different forces the plane will experiment, besides gravity)
    //  - [?] Observer for the camera (shared pointer)
    //  - [?] Materials
};

#endif
