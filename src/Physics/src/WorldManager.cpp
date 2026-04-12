#include "WorldManager.h"
#include "GraphicsWorldManager.h"
#include "ParticleSystem.h"
#include "PhysicsManager.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics {
//--------------------------------------------------------------------------
// WorldManager class

WorldManager::WorldManager() {}

WorldManager::~WorldManager() {}

Entity *WorldManager::addEntity(std::unique_ptr<Entity> entity) {
    // NOTE: Call this as
    // auto entity = std::make_unique<Entity>(...);
    // world.addEntity(std::move(entity));

    Entity *raw = entity.get();
    mEntities.push_back(std::move(entity));

    registerEntityInManagers(mEntities.back().get());

    return raw;
}

ParticleSystem *WorldManager::addParticleSystem(std::unique_ptr<ParticleSystem> particleSystem) {
    ParticleSystem *raw = particleSystem.get();
    mParticleSystems.push_back(std::move(particleSystem));

    registerParticleSystemInManagers(mParticleSystems.back().get());

    return raw;
}

Terrain *WorldManager::addTerrain(std::unique_ptr<Terrain> terrain) {
    Terrain *raw = terrain.get();
    mTerrain = std::move(terrain);

    registerTerrainInManagers(mTerrain.get());

    return raw;
}

std::vector<GLObjectWithMaterial> &WorldManager::getListOfObjectsWithGraphics() {
    return mGraphicsManager.getListOfObjects();
};

PhysicsManager &WorldManager::getPhysicsManager() { return mPhysicsManager; }

GraphicsWorldManager &WorldManager::getGraphicsManager() { return mGraphicsManager; }

void WorldManager::simulationStep(float deltaTime) { mPhysicsManager.step(deltaTime); }

// Draw the objects in the current frame, to the G-buffer
void WorldManager::draw() { mGraphicsManager.draw(); }

void WorldManager::registerEntityInManagers(Entity *entity) {
    if (entity->hasPhysics()) {
        mPhysicsManager.registerEntity(entity);
    }

    if (entity->hasGeometry()) {
        mGraphicsManager.registerObjectAndMaterial(entity->getGeometry(), entity->getMaterial());
    }
}

void WorldManager::registerParticleSystemInManagers(ParticleSystem *particleSystem) {
    mPhysicsManager.registerParticleSystem(particleSystem);

    if (particleSystem->hasGeometry()) {
        mGraphicsManager.registerObjectAndMaterial(particleSystem->getGeometry(), particleSystem->getMaterial());
    }
}

void WorldManager::registerTerrainInManagers(Terrain *terrain) {
    mPhysicsManager.registerTerrain(terrain);

    mGraphicsManager.registerTerrainAndMaterial(mTerrain->getTerrainPatch(), mTerrain->getMaterial());
}

} // namespace Physics
