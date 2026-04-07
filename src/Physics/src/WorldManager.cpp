#include "WorldManager.h"

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

void WorldManager::addTerrain(std::unique_ptr<Terrain> terrain) {
    mTerrain = std::move(terrain);
}

// Draw the objects in the current frame, to the G-buffer
// void CollisionWorld::draw( Shader& defaultShader )
void WorldManager::draw() {
    // TODO: Delegate this to the graphics manager

    // TODO: Draw the terrain first (also in the graphics manager)
    mTerrain->draw();

    for (auto collisionBody : mCollisionBodies) {
        // collisionBody->draw( defaultShader );
        collisionBody->draw();
    }
}

void WorldManager::registerEntityInManagers(Entity *entity) {
    if (entity->hasPhysics()) {
        mPhysicsManager.registerEntity(entity);
    }

    if (entity->getGeometry() != nullptr) {
        // TODO: Register the geometry in mGraphicsManager
    }
}

} // namespace Physics
