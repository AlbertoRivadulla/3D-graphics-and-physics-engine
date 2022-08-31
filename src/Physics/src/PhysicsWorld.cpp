#include "PhysicsWorld.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics
{
    //--------------------------------------------------------------------------
    // CollisionWorld class

    // Constructor
    CollisionWorld::CollisionWorld()
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
            collisionBody->draw( defaultShader );
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
        for ( auto body : mRigidBodies )
            delete body;
    }

    // Add a RigidBody
    void DynamicsWorld::addRigidBody( RigidBody* rigidBody )
    {
        mRigidBodies.push_back( rigidBody );
    }

    // Update the objects in the current frame
    void DynamicsWorld::step( float deltaTime )
    {
        // std::cout << "step" << std::endl;


        // Move the dynamic objects
        for ( auto body : mRigidBodies )
            body->updateMovement( deltaTime );
    }

    // Draw the objects in the current frame, to the G-buffer
    void DynamicsWorld::draw( Shader& defaultShader )
    {
        // std::cout << "draw" << std::endl;

        // Draw all the collision bodies
        for ( auto collisionBody : mCollisionBodies )
            collisionBody->draw( defaultShader );

        // Draw all the rigid bodies
        for ( auto rigidBody : mRigidBodies )
            rigidBody->draw( defaultShader );
    }
}
