#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include "Physics.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics
{
    // The following class manages objects with collisions (CollisionBody)
    class CollisionWorld
    {
        public:
            // Constructor
            CollisionWorld();

            // Destructor
            ~CollisionWorld();

            // Add a CollisionBody
            void addCollisionBody( CollisionBody* collisionBody );

            // Draw the objects in the current frame, to the G-buffer
            void draw( Shader& defaultShader );

        protected:
            // Vector of pointer to CollisionBody objects
            std::vector<CollisionBody*> mCollisionBodies;

            // Used to slow down simulations
            int mCounter;
    };

    // The following class manages objects with collisions and dynamics (RigidBody)
    class DynamicsWorld : public CollisionWorld
    {
        public:
            // Constructor
            DynamicsWorld();

            // Destructor
            ~DynamicsWorld();

            // Add a RigidBody
            void addRigidBody( RigidBody* rigidBody );

            // Update the objects in the current frame
            void step( float deltaTime );
            //
            // // Draw the objects in the current frame, to the G-buffer
            // void draw( Shader& defaultShader );

        private:
            // Vector of pointer to RigidBody objects
            std::vector<RigidBody*> mRigidBodies;
    };
}

#endif
