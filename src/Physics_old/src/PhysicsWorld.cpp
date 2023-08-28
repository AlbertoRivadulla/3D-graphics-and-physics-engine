#include "PhysicsWorld.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics
{
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
    void CollisionWorld::draw( Shader& defaultShader )
    {
        // Draw all the collision bodies
        for ( auto collisionBody : mCollisionBodies )
        {
            collisionBody->draw( defaultShader );
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

    // Update the objects in the current frame
    void DynamicsWorld::step( float deltaTime )
    {
        /*
           The steps of the simulation are:
            - Apply forces
            - Update positions and velocities
            - Detect collisions
            - Solve constraints
        */

        // Update the movement after a certain amount of frames
        if ( mCounter++ == 10 )
        {
            std::cout << "step" << std::endl;
            mCounter = 0;

            // Apply forces on the objects
            //
            //

            // Move the dynamic objects
            for ( auto body : mRigidBodies )
                body->updateMovement( deltaTime );

            // Check for collisions between pairs of objects
            for ( auto bodyA : mCollisionBodies )
            {
                for ( auto bodyB : mCollisionBodies )
                {
                    // If they are the same, break the loop
                    if ( bodyA == bodyB )
                        break;
                    // If they are not the same, check for collisions between their
                    // AABB
                    // if ( bodyA->mCollider->checkCollisionAABB( bodyB->mCollider ) )
                    if ( bodyA->mCollider->findCollision( bodyB->mCollider ) )
                    {
                        std::cout << "Collision between" << std::endl;

                        // Check for collision between their finer colliders
                    }

                }
            }
            //
            //


            // Solve constraints
            //
            //
        }
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
