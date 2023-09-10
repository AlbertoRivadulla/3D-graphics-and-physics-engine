#include "ForceGenerator.h"
#include "utils.h"

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
        float speed = glm::length( velocity );
        // Apply the force
        rigidBody->addForce( - velocity * ( mK1 + mK2 * speed ) );
    }

    //--------------------------------------------------------------------------
    // Class SpringForceGenerator
    SpringForceGenerator::SpringForceGenerator( RigidBody* other, float springConst, 
                                                float dampingCoeff, float restLength ) :
        mOtherBody { other }, mSpringConst { springConst }, 
        mDampingCoeff { dampingCoeff }, mRestLength { restLength }
    {
    }

    void SpringForceGenerator::updateForce( RigidBody* rigidBody, float deltaTime )
    {
        glm::vec3 separation = rigidBody->getPosition() - mOtherBody->getPosition();
        float distance = glm::length( separation );
        // Normalize the separation
        separation /= distance;
        // Force of the spring
        float magnitude = mSpringConst * ( mRestLength - distance );
        // Relative speed on the direction of separation
        float relativeSpeed = glm::dot( ( rigidBody->getVelocity() - mOtherBody->getVelocity() ), separation );
        // relativeSpeed = 0.f;

        // The total force is the one due to the separation plus the damping proportional
        // to the relative speed
        rigidBody->addForce( ( magnitude - mDampingCoeff * relativeSpeed ) * separation);


        // glm::vec3 force = rigidBody->getPosition() - mOtherBody->getPosition();
        // float magnitude = glm::length( force );
        // magnitude = mSpringConst * fabs( magnitude - mRestLength );
        // force /= glm::length(force);
        // force *= - magnitude;
        // rigidBody->addForce( force );

    }

    //--------------------------------------------------------------------------
    // Class BungeeForceGenerator
    BungeeForceGenerator::BungeeForceGenerator( RigidBody* other, float springConst, 
                                                float restLength ) :
        mOtherBody { other }, mSpringConst { springConst }, mRestLength { restLength }
    {
    }

    void BungeeForceGenerator::updateForce( RigidBody* rigidBody, float deltaTime )
    {
        glm::vec3 separation = rigidBody->getPosition() - mOtherBody->getPosition();
        float distance = glm::length( separation );
        // Force of the spring
        float magnitude = mSpringConst * ( mRestLength / distance - 1.f );
        // Only place this force if the magnitude is smaller than zero (the bungee is stretched)
        if ( magnitude < 0 )
            rigidBody->addForce( magnitude * separation);
    }
}
