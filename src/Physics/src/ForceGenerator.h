#ifndef FORCEGENERATOR_H
#define FORCEGENERATOR_H

#include "GLBase.h"
#include "PhysicsBody.h"

using namespace GLBase;

namespace Physics
{
    // Virtual force generator
    class ForceGenerator
    {
        public:
            // Calculate and apply the corresponding force to a RigidBody
            virtual void updateForce( RigidBody* rigidBody, float deltaTime ) = 0;
    };

    // Gravity force generator
    class GravityForceGenerator : public ForceGenerator
    {
        public:
            GravityForceGenerator( const glm::vec3& gravity );

            virtual void updateForce( RigidBody* rigidBody, float deltaTime );

        private:
            glm::vec3 mGravity;
    };

    // Drag force generator
    class DragForceGenerator : public ForceGenerator
    {
        public:
            DragForceGenerator( float k1, float k2 );

            virtual void updateForce( RigidBody* rigidBody, float deltaTime );

        private:
            // Coefficients for velocity and velocity square
            float mK1;
            float mK2;
    };
}

#endif
