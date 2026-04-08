#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include <vector>
#include <memory>
#include "PhysicsManager.h"
#include "Entity.h"
#include "ForceGenerator.h"
#include "Terrain.h"
#include "GraphicsManager.h"

using namespace GLBase;

namespace Physics {
class WorldManager {
public:
    WorldManager();

    ~WorldManager();

    void addEntity(std::unique_ptr<Entity> entity);

    void addTerrain(std::unique_ptr<Terrain> terrain);

    void simulationStep(float deltaTime);

    // Draw the objects in the current frame, to the G-buffer
    void draw();

private:
    // List of all entities
    std::vector<std::unique_ptr<Entity>> mEntities;

    std::unique_ptr<Terrain> mTerrain;

    PhysicsManager mPhysicsManager;
    GLGeometry::GraphicsManager mGraphicsManager;

    void registerEntityInManagers(Entity *entity);
};

} // namespace Physics

#endif
