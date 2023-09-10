#include "ForceGenerator.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics
{
    //--------------------------------------------------------------------------
    // Class GravityForceGenerator

    GravityForceGenerator::GravityForceGenerator( const glm::vec3& gravity ) :
        mGravity { gravity }
    {
    }

    void GravityForceGenerator::updateForce( RigidBody* rigidBody, float deltaTime )
    {
        if ( rigidBody->hasInfiniteMass() )
            return;

        // Apply the force scaled by the mass
        rigidBody->addForce( mGravity * rigidBody->getMass() );
    }

    //--------------------------------------------------------------------------
    // Class DragForceGenerator
    DragForceGenerator::DragForceGenerator( float k1, float k2 ) :
        mK1 { k1 }, mK2 { k2 }
    {
    }

    void DragForceGenerator::updateForce( RigidBody* rigidBody, float deltaTime )
    {
        // Get the velocity of the particle
        glm::vec3 velocity = rigidBody->getVelocity();
        // Magnitude of the velocity
        float speed = velocity.length();
        // Apply the force
        rigidBody->addForce( - velocity * ( mK1 + mK2 * speed ) );
    }
}
