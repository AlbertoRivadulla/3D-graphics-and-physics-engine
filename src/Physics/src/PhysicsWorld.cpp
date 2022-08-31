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

        std::cout << "Initializing collisionworld" << std::endl;

    }

    // Destructor
    CollisionWorld::~CollisionWorld()
    {
        std::cout << "Destroying collisionworld" << std::endl;

        for ( auto body : mCollisionBodies )
            delete body;

    }

    // Add a CollisionBody
    void CollisionWorld::addCollisionBody( CollisionBody* collisionBody )
    {
        mCollisionBodies.push_back( collisionBody );
    }

    // Draw the objects in the current frame, to the G-buffer
    void CollisionWorld::draw( const Shader& defaultShader )
    {

    }

    //--------------------------------------------------------------------------
    // DynamicsWorld class

    // Constructor
    DynamicsWorld::DynamicsWorld()
    {
        
        std::cout << "Initializing dynamicsworld" << std::endl;

    }

    // Destructor
    DynamicsWorld::~DynamicsWorld()
    {
        std::cout << "Destroying dynamicsworld" << std::endl;

        for ( auto body : mRigidBodies )
            delete body;

    }

    // Add a RigidBody
    void DynamicsWorld::addRigidBody( RigidBody* rigidBody )
    {
        mRigidBodies.push_back( rigidBody );
    }

    // Update the objects in the current frame
    void DynamicsWorld::step()
    {
    }

    // Draw the objects in the current frame, to the G-buffer
    void DynamicsWorld::draw( Shader& defaultShader )
    {
        std::cout << "Drawing from DynamicsWorld" << std::endl;

        // Draw all the collision bodies
        for ( auto collisionBody : mCollisionBodies )
            collisionBody->draw( defaultShader );

        // Draw all the rigid bodies
        for ( auto rigidBody : mRigidBodies )
            rigidBody->draw( defaultShader );
    }
}
