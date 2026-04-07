#include "PhysicsManager.h"

namespace Physics {

//--------------------------------------------------------------------------
// BodyForceRegistry class

// Register a pair body-force
void BodyForceRegistry::addBodyForce(RigidBody *body, ForceGenerator *force) {
    mRegistrations.push_back(BodyForceRegistration(body, force));
}

// Remove a pair body-force
// If the pair is not registrated, this will not do anything
void BodyForceRegistry::removeBodyForce(RigidBody *body,
                                        ForceGenerator *force) {
    // Look for the registration
    for (Registry::iterator regIter = mRegistrations.begin();
         regIter != mRegistrations.end(); ++regIter) {
        if (regIter->forceGenerator == force && regIter->rigidBody == body) {
            mRegistrations.erase(regIter);
            return;
        }
    }
}

void BodyForceRegistry::clear() { mRegistrations.clear(); }

// Call the force generators to update the forces on the particles
void BodyForceRegistry::applyForces(float deltaTime) {
    for (Registry::iterator regIter = mRegistrations.begin();
         regIter != mRegistrations.end(); ++regIter) {
        regIter->forceGenerator->updateForce(regIter->rigidBody, deltaTime);
    }
}

//--------------------------------------------------------------------------
// DynamicsWorld class

PhysicsManager::PhysicsManager() {
    // NOTE: This is here only for debugging purposes
    mCounter = 0;
}

PhysicsManager::~PhysicsManager() {}

void PhysicsManager::registerEntity(Entity *entity) {
    mEntitiesWithPhysics.push_back(entity);

    // TODO: Should this distinguish which objects have colliders and which have rigid bodies? Place them in different lists?
}

// Register a pair body-force
void PhysicsManager::addBodyForce(RigidBody *body, ForceGenerator *force) {
    mBodyForceRegistry.addBodyForce(body, force);
}

// Remove a pair body-force
void PhysicsManager::removeBodyForce(RigidBody *body, ForceGenerator *force) {
    mBodyForceRegistry.removeBodyForce(body, force);
}

void PhysicsManager::addParticleSystem(ParticleSystem *particleSystem) {
    // TODO: Where should this be placed, in order for the particle system to have collisions?
    mParticleSystems.push_back(particleSystem);
    mCollisionBodies.push_back(particleSystem);
}

// Update the objects in the current frame
void PhysicsManager::step(float deltaTime) {
    assert(deltaTime > 0.f);

    /*
       The steps of the simulation are:
        - Apply forces
        - Update positions and velocities
        - Detect collisions
        - Solve constraints
    */

    // NOTE: These ifs are here only for debugging
    // Update the movement after a certain amount of frames
    // if ( mCounter++ == 10 )
    if (mCounter++ == 0) {
        mCounter = 0;

        // Apply forces on the objects
        mBodyForceRegistry.applyForces(deltaTime);

        // Move the dynamic objects
        for (auto body : mRigidBodies)
            body->integrate(deltaTime);

        // Check for collisions between pairs of objects
        // for ( auto bodyA : mCollisionBodies )
        // {
        //     for ( auto bodyB : mCollisionBodies )
        //     {
        //         // If they are the same, break the loop
        //         if ( bodyA == bodyB )
        //             break;
        //         // If they are not the same, check for collisions between
        //         their
        //         // AABB
        //         // if ( bodyA->mCollider->checkCollisionAABB(
        //         bodyB->mCollider ) ) if ( bodyA->mCollider->findCollision(
        //         bodyB->mCollider ) )
        //         {
        //             LOG_INFO("Collision between");
        //
        //             // Check for collision between their finer colliders
        //         }
        //
        //     }
        // }
        //
        //

        // Check also for collisions with the terrain

        // Solve constraints
        //
        //
    }

    // Update the particle systems
    for (auto particleSystem : mParticleSystems)
        particleSystem->integrate(deltaTime);
}

// // Draw the objects in the current frame, to the G-buffer
// void DynamicsWorld::draw( Shader& defaultShader )
// {
//     // Draw all the collision bodies
//     for ( auto collisionBody : mCollisionBodies )
//         collisionBody->draw( defaultShader );
//
//     // Draw all the rigid bodies
//     for ( auto rigidBody : mRigidBodies )
//         rigidBody->draw( defaultShader );
// }

} // namespace Physics
