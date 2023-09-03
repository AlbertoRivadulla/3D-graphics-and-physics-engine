#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "GLBase.h"
#include "GLGeometry.h"
#include "Colliders.h"
#include "PhysicsBody.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics
{
    // Class for the particle system
    class ParticleSystem : public CollisionBody
    {
        public:
            // Constructor
            ParticleSystem( glm::vec3 position, glm::vec3 scale,
                       float rotationAngle, glm::vec3 rotationAxis,
                       float mass, glm::vec3 velocity = {0.f, 0.f, 0.f} );

            // Set the geometry of a single particle
            // Add geometrical object, and copy it to the list of elementary objects of
            // the GLSandbox class
            // Also sets the geometry of a single particle
            void setParticleGeometry( GLElemObject* particleObjectPtr,
                                      std::vector<GLElemObject*>& elemObjs, 
                                      Shader* GPassShader );

            // Set gravity of particles
            void setParticleGravity( glm::vec3 gravity );

            // Set velocity and acceleration
            void setVelocity( glm::vec3 velocity );
            void setGravity( glm::vec3 gravity );

            // Set mass 
            void setMass( float mass );
            void setInvMass( float invMass );

            // Set velocity damping
            void setDamping( float damping );

            // Add a single particle
            void addParticle( glm::vec3 velocity, glm::vec3 scale, float maxAge, 
                              Material* material );

            // Integrate forward in time by the given duration
            void integrate( float deltaTime );

        private:
            // Particle system object for the graphics
            GLParticleSystem* mParticleSystemGL;
            // Pointer to the list of particles
            std::list<GLParticle*>* mParticles;
            // std::list<GLParticle>* mParticles;
            // Gravity of particles
            glm::vec3 mParticleGravity;

            // Number of particles
            int mParticleCount;

            // Variables for dynamics
            float mMass;
            float mMassInver;
            glm::vec3 mVelocity;
            // glm::vec3 mAcceleration;
            // Gravity acceleration
            glm::vec3 mGravity;

            // Damping applied to linear motion, to ensure objects are not accelerated
            // due to numerical inaccuracies
            float mDamping;

            // Accumulator for forces
            glm::vec3 mForceAccum;
            // glm::vec3 mTorqueAccum;
    };
}

#endif
