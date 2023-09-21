#include "ParticleSystem.h"
#include "GLParticleSystem.h"
#include "utils.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics
{

    // Constructor
    ParticleSystem::ParticleSystem( Shader& shader, glm::vec3 position, glm::vec3 scale,
               float rotationAngle, glm::vec3 rotationAxis,
               float mass, glm::vec3 velocity ) :
        CollisionBody( position, scale, rotationAngle, rotationAxis ),      // Initialize the base class explicitly
        mGPassShader { &shader },
        mParticleCount { 0 },
        mMass { mass }, 
        mMassInver { 1.f / mass },
        mVelocity { velocity }, 
        mGravity { glm::vec3( 0.f, 0.f, 0.f ) },
        mDamping { 0.995f },
        mForceAccum { glm::vec3( 0.f, 0.f, 0.f ) }
    {
        // Placeholder for the material
        mMaterial = new Material( shader, {0.f, 0.f, 0.f}, 0.f );
    }

    // Set the geometry of a single particle
    // Add geometrical object, and copy it to the list of elementary objects of
    // the GLSandbox class
    // Also sets the geometry of a single particle
    void ParticleSystem::setParticleGeometry( GLElemObject* particleObjectPtr,
                                              std::vector<GLElemObject*>& elemObjs, 
                                              Shader* GPassShader )
    {
        // Add a GLParticleSystem object, with the given geometry
        mParticleSystemGL = new GLParticleSystem( particleObjectPtr, GPassShader );
        mGeometryObject = mParticleSystemGL;

        // Copy a pointer to the list of particles
        mParticles = mParticleSystemGL->getPointerToListOfParticles();

        // Store the pointer also in the list of elementary objects in the scene
        elemObjs.push_back( mParticleSystemGL );

        // Compute the model matrix of the geometry object
        computeModelMatrix();
    }

    // Set gravity of particles
    void ParticleSystem::setParticleGravity( glm::vec3 gravity )
    {
        mParticleGravity = gravity;
    }

    // Set velocity and acceleration
    void ParticleSystem::setVelocity( glm::vec3 velocity )
    {
        mVelocity = velocity;
    }
    void ParticleSystem::setGravity( glm::vec3 gravity )
    {
        mGravity = gravity;
    }

    // Set velocity damping
    void ParticleSystem::setDamping( float damping )
    {
        mDamping = damping;
    }

    // Set mass 
    void ParticleSystem::setMass( float mass )
    {
        mMass = mass;
        mMassInver = 1.f / mass;
    }
    void ParticleSystem::setInvMass( float invMass )
    {
        mMassInver = invMass;
        if ( invMass == 0.f )
            mMass = -1.f;
        else
            mMass = 1.f / invMass;
    }

    // Add a single particle
    void ParticleSystem::addParticle( glm::vec3 velocity, glm::vec3 scale, 
                                      float maxAge, Material* material )
    {
        mParticles->push_back( new GLParticle( mPosition, mVelocity + velocity,
                                               scale, maxAge, material ) );
        mParticleCount += 1;
    }

    // Integrate forward in time by the given duration
    void ParticleSystem::integrate( float deltaTime )
    {
        // Add random particle
        if ( mParticleCount < 100 )
            addParticle( glm::vec3( -2.f + 4.f*GLUtils::getRandom0To1(), 15.f*GLUtils::getRandom0To1(), -2.f + 4.f*GLUtils::getRandom0To1() ),
                           0.5f * glm::vec3( 1.f, 1.f, 1.f ),
                           1.f + 5.f * GLUtils::getRandom0To1(),
                           new Material( *mGPassShader, { GLUtils::getRandom0To1(), GLUtils::getRandom0To1(), GLUtils::getRandom0To1() }, 0.5f, 1.f ) );
        
        if ( mParticleCount == 0 )
            return;

        for ( std::list<GLParticle*>::iterator particleItr = mParticles->begin(); 
              particleItr != mParticles->end(); ++particleItr )
        {
            GLParticle* particlePtr = *particleItr;
            // If the age of the particle is too large, delete it
            if ( particlePtr->age > particlePtr->maxAge )
            {
                // Delete the particle and move the iterator to the next one
                mParticles->erase(particleItr++);
                mParticleCount -= 1;
            }

            // Add deltaTime to the age of the particle
            particlePtr->age += deltaTime;

            // Compute acceleration, update linear velocity, and update position
            glm::vec3 resultingAcc = mParticleGravity;
            particlePtr->velocity += resultingAcc * deltaTime;
            particlePtr->velocity *= powf( mDamping, deltaTime );
            particlePtr->position += particlePtr->velocity * deltaTime;

            // Update its model matrix
            particlePtr->computeModelMatrix();
        }

        // Compute acceleration from the force
        glm::vec3 resultingAcc = mGravity + mForceAccum * mMassInver;
        // Update linear velocity
        mVelocity += resultingAcc * deltaTime;
        // Drag on the velocity, so it does not increase due to numerical errors
        mVelocity *= powf( mDamping, deltaTime );

        // Update the position
        mPosition += mVelocity * deltaTime;

        // Update the model matrix 
        // computeModelMatrix( mPosition, mRotationMatrix, mScale );
        computeModelMatrix();

        // // Move the collider
        // mCollider->moveCollider( mModelMatrix );

        // Reset the net force and torque on the object
        mForceAccum = glm::vec3( 0.f, 0.f, 0.f );
        // mTorqueAccum = glm::vec3( 0.f, 0.f, 0.f );
    }
}
