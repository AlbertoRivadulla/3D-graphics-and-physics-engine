#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include "Physics.h"
#include "PhysicsBody.h"
#include "ForceGenerator.h"
#include "Terrain.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics
{
    // Registry for the forces that apply to each body in the world
    class BodyForceRegistry
    {
        protected:
            // One force and the particle it is applied to
            struct BodyForceRegistration
            {
                RigidBody* rigidBody;
                ForceGenerator* forceGenerator;
                BodyForceRegistration( RigidBody* body, ForceGenerator* force )
                {
                    rigidBody = body;
                    forceGenerator = force;
                }
            };

            // List of all registrations
            // I think this works the same for a vector or a linked list
            // typedef std::vector<BodyForceRegistration> Registry;
            typedef std::list<BodyForceRegistration> Registry;
            Registry mRegistrations;

        public:
            // Register a pair body-force
            void addBodyForce( RigidBody* body, ForceGenerator* force );

            // Remove a pair body-force
            // If the pair is not registrated, this will not do anything
            void removeBodyForce( RigidBody* body, ForceGenerator* force );

            // Clear all the registrations
            void clear();

            // Call the force generators to update the forces on the particles
            void applyForces( float deltaTime );
    };

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

            // Add a terrain
            void addTerrain( Terrain* terrain );

            // Draw the objects in the current frame, to the G-buffer
            // void draw( Shader& defaultShader );
            void draw();

            // Draw the terrain
            void drawTerrain();

        protected:
            // Vector of pointer to CollisionBody objects
            std::vector<CollisionBody*> mCollisionBodies;
            // Vector of pointer to CollisionBody objects that are not drawn
            std::vector<CollisionBody*> mCollisionBodiesNotDrawn;

            // Terrain
            Terrain* mTerrain;

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

            // Add a RigidBody that is not drawn
            void addRigidBodyNotDrawn( RigidBody* rigidBody );

            // Register a pair body-force
            void addBodyForce( RigidBody* body, ForceGenerator* force );

            // Remove a pair body-force
            void removeBodyForce( RigidBody* body, ForceGenerator* force );

            // Add a ParticleSystem
            void addParticleSystem( ParticleSystem* particleSystem );

            // Update the objects in the current frame
            void step( float deltaTime );

            // // Draw the objects in the current frame, to the G-buffer
            // void draw( Shader& defaultShader );

        private:
            // Vector of pointers to RigidBody objects
            std::vector<RigidBody*> mRigidBodies;
            // Vector of pointers to ParticleSystem objects
            std::vector<ParticleSystem*> mParticleSystems;

            // Registry of the forces applied to each body
            BodyForceRegistry mBodyForceRegistry;
    };
}

#endif
