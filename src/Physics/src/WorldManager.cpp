#include "WorldManager.h"
#include "ParticleSystem.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics {
//--------------------------------------------------------------------------
// WorldManager class

WorldManager::WorldManager() {}

WorldManager::~WorldManager() {}

void WorldManager::addEntity(std::unique_ptr<Entity> entity) {
    // NOTE: Call this as
    // auto entity = std::make_unique<Entity>(...);
    // world.addEntity(std::move(entity));

    mEntities.push_back(std::move(entity));

    registerEntityInManagers(mEntities.back().get());
}

void WorldManager::addParticleSystem(
    std::unique_ptr<ParticleSystem> particleSystem) {
    mParticleSystems.push_back(std::move(particleSystem));

    registerParticleSystemInManagers(mParticleSystems.back().get());
}

void WorldManager::addTerrain(std::unique_ptr<Terrain> terrain) {
    mTerrain = std::move(terrain);

    registerTerrainInManagers(mTerrain.get());
}

void WorldManager::simulationStep(float deltaTime) {
    mPhysicsManager.step(deltaTime);

    // TODO: Register the terrain in mGraphicsManager
}

// Draw the objects in the current frame, to the G-buffer
// void CollisionWorld::draw( Shader& defaultShader )
void WorldManager::draw() { mGraphicsManager.draw(); }

void WorldManager::drawTerrain() {
    // TODO: Move this to the GraphicsManager when it has a pointer to the
    // graphics part of the terrain object.
    mTerrain->draw();
}

void WorldManager::registerEntityInManagers(Entity *entity) {
    if (entity->hasPhysics()) {
        mPhysicsManager.registerEntity(entity);
    }

    if (entity->hasGeometry()) {
        mGraphicsManager.registerObjectAndMaterial(entity->getGeometry(),
                                                   entity->getMaterial());
    }
}

void WorldManager::registerParticleSystemInManagers(
    ParticleSystem *particleSystem) {
    mPhysicsManager.registerParticleSystem(particleSystem);

    if (particleSystem->hasGeometry()) {
        mGraphicsManager.registerObjectAndMaterial(
            particleSystem->getGeometry(), particleSystem->getMaterial());
    }
}

void WorldManager::registerTerrainInManagers(Terrain *terrain) {
    // TODO: The terrain should also be registered in the Graphics manager
    mPhysicsManager.registerTerrain(terrain);
}

} // namespace Physics
