#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include <vector>
#include <memory>
#include "Entity.h"
#include "Terrain.h"
#include "ParticleSystem.h"
#include "PhysicsManager.h"
#include "GraphicsWorldManager.h"

using namespace GLBase;

namespace Physics {
class WorldManager {
public:
    WorldManager();

    ~WorldManager();

    Entity *addEntity(std::unique_ptr<Entity> entity);

    ParticleSystem *addParticleSystem(std::unique_ptr<ParticleSystem> particleSystem);

    Terrain *addTerrain(std::unique_ptr<Terrain> terrain);

    std::vector<GLObjectWithMaterial> &getListOfObjectsWithGraphics();

    PhysicsManager &getPhysicsManager();

    GraphicsWorldManager &getGraphicsManager();

    void simulationStep(float deltaTime);

    // Draw the objects in the current frame, to the G-buffer
    void draw();

private:
    // This class has ownership of all the following
    std::vector<std::unique_ptr<Entity>> mEntities;

    std::vector<std::unique_ptr<ParticleSystem>> mParticleSystems;

    std::unique_ptr<Terrain> mTerrain;

    PhysicsManager mPhysicsManager;
    GLGeometry::GraphicsWorldManager mGraphicsManager;

    void registerEntityInManagers(Entity *entity);

    void registerParticleSystemInManagers(ParticleSystem *particleSystem);

    void registerTerrainInManagers(Terrain *terrain);
};

} // namespace Physics

#endif
