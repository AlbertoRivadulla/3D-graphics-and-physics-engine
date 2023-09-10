#include "PhysicsWorld.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics
{
    //--------------------------------------------------------------------------
    // BodyForceRegistry class

    // Register a pair body-force
    void BodyForceRegistry::addBodyForce( RigidBody* body, ForceGenerator* force )
    {
        mRegistrations.push_back( BodyForceRegistration( body, force ) );
    }

    // Remove a pair body-force
    // If the pair is not registrated, this will not do anything
    void BodyForceRegistry::removeBodyForce( RigidBody* body, ForceGenerator* force )
    {
        // Look for the registration
        for ( Registry::iterator regIter = mRegistrations.begin(); 
              regIter != mRegistrations.end(); ++regIter )
        {
            if ( regIter->forceGenerator == force && regIter->rigidBody == body )
            {
                mRegistrations.erase( regIter );
                return;
            }
        }
    }

    // Clear all the registrations
    void BodyForceRegistry::clear()
    {
        mRegistrations.clear();
    }

    // Call the force generators to update the forces on the particles
    void BodyForceRegistry::applyForces( float deltaTime )
    {
        for ( Registry::iterator regIter = mRegistrations.begin(); 
              regIter != mRegistrations.end(); ++regIter )
        {
            regIter->forceGenerator->updateForce( regIter->rigidBody, deltaTime );
        }
    }


    //--------------------------------------------------------------------------
    // CollisionWorld class

    // Constructor
    CollisionWorld::CollisionWorld() : 
        mCounter { 0 }
    {

    }

    // Destructor
    CollisionWorld::~CollisionWorld()
    {
        for ( auto body : mCollisionBodies )
            delete body;
    }

    // Add a CollisionBody
    void CollisionWorld::addCollisionBody( CollisionBody* collisionBody )
    {
        mCollisionBodies.push_back( collisionBody );
    }

    // Draw the objects in the current frame, to the G-buffer
    // void CollisionWorld::draw( Shader& defaultShader )
    void CollisionWorld::draw()
    {
        // Draw all the collision bodies
        for ( auto collisionBody : mCollisionBodies )
        {
            // collisionBody->draw( defaultShader );
            collisionBody->draw();
        }
    }

    //--------------------------------------------------------------------------
    // DynamicsWorld class

    // Constructor
    DynamicsWorld::DynamicsWorld()
    {

    }

    // Destructor
    DynamicsWorld::~DynamicsWorld()
    {
        // No need to deallocate the rigidbodies, as they are also located at the
        // vector of collision bodies
        // for ( auto body : mRigidBodies )
        //     delete body;
    }

    // Add a RigidBody
    void DynamicsWorld::addRigidBody( RigidBody* rigidBody )
    {
        mRigidBodies.push_back( rigidBody );

        // Add it also to the list of collision bodies, to check for collisions and
        // draw
        mCollisionBodies.push_back( rigidBody );
    }

    // Add a RigidBody that is not drawn
    void DynamicsWorld::addRigidBodyNotDrawn( RigidBody* rigidBody )
    {
        mRigidBodies.push_back( rigidBody );

        // Add it also to the list of collision bodies, to check for collisions and
        // draw
        mCollisionBodiesNotDrawn.push_back( rigidBody );
    }

    // Register a pair body-force
    void DynamicsWorld::addBodyForce( RigidBody* body, ForceGenerator* force )
    {
        mBodyForceRegistry.addBodyForce( body, force );
    }

    // Remove a pair body-force
    void DynamicsWorld::removeBodyForce( RigidBody* body, ForceGenerator* force )
    {
        mBodyForceRegistry.removeBodyForce( body, force );
    }

    // Add a ParticleSystem
    void DynamicsWorld::addParticleSystem( ParticleSystem* particleSystem )
    {
        mParticleSystems.push_back( particleSystem );
        mCollisionBodies.push_back( particleSystem );
    }

    // Update the objects in the current frame
    void DynamicsWorld::step( float deltaTime )
    {
        assert( deltaTime > 0.f );

        /*
           The steps of the simulation are:
            - Apply forces
            - Update positions and velocities
            - Detect collisions
            - Solve constraints
        */

        // Update the movement after a certain amount of frames
        // if ( mCounter++ == 10 )
        if ( mCounter++ == 0 )
        {
            // std::cout << "step" << std::endl;
            mCounter = 0;

            // Apply forces on the objects
            mBodyForceRegistry.applyForces( deltaTime );

            // Move the dynamic objects
            for ( auto body : mRigidBodies )
                body->integrate( deltaTime );

            // Check for collisions between pairs of objects
            // for ( auto bodyA : mCollisionBodies )
            // {
            //     for ( auto bodyB : mCollisionBodies )
            //     {
            //         // If they are the same, break the loop
            //         if ( bodyA == bodyB )
            //             break;
            //         // If they are not the same, check for collisions between their
            //         // AABB
            //         // if ( bodyA->mCollider->checkCollisionAABB( bodyB->mCollider ) )
            //         if ( bodyA->mCollider->findCollision( bodyB->mCollider ) )
            //         {
            //             std::cout << "Collision between" << std::endl;
            //
            //             // Check for collision between their finer colliders
            //         }
            //
            //     }
            // }
            //
            //


            // Solve constraints
            //
            //
        }

        // Update the particle systems
        for ( auto particleSystem : mParticleSystems )
            particleSystem -> integrate( deltaTime );
    }

    // // Draw the objects in the current frame, to the G-buffer
    // void DynamicsWorld::draw( Shader& defaultShader )
    // {
    //     // std::cout << "draw" << std::endl;
    //
    //     // Draw all the collision bodies
    //     for ( auto collisionBody : mCollisionBodies )
    //         collisionBody->draw( defaultShader );
    //
    //     // Draw all the rigid bodies
    //     for ( auto rigidBody : mRigidBodies )
    //         rigidBody->draw( defaultShader );
    // }
}
