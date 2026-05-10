#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include <vector>
#include <memory>
#include "Entity.h"
#include "Terrain.h"
#include "ParticleSystem.h"
#include "PhysicsManager.h"
#include "GraphicsWorldManager.h"
#include "interfaces.h"
#include "src/EntityObserver.h"

using namespace GLBase;

namespace Physics {
class WorldManager {
public:
    WorldManager();

    ~WorldManager();

    Entity *addEntity(std::unique_ptr<Entity> entity);

    ParticleSystem *addParticleSystem(std::unique_ptr<ParticleSystem> particleSystem);

    Terrain *addTerrain(std::unique_ptr<Terrain> terrain);

    template <typename T, typename... Args> std::shared_ptr<T> addObserver(Args &&...args) {
        auto observer = std::make_shared<T>(std::forward<Args>(args)...);
        mEntityObservers.push_back(observer);

        // The caller can store this in a shared_ptr or weak_ptr
        return observer;
    }

    void removeObserver(std::shared_ptr<IEntityObserverBase> observer);

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

    std::vector<std::shared_ptr<IEntityObserverBase>> mEntityObservers;

    void registerEntityInManagers(Entity *entity);

    void registerParticleSystemInManagers(ParticleSystem *particleSystem);

    void registerTerrainInManagers(Terrain *terrain);
};

} // namespace Physics

#endif
