#include "ParticleSystem.h"
#include "GLParticleSystem.h"
#include "utils.h"
#include <memory>

using namespace GLGeometry;
using namespace GLBase;

namespace Physics {

ParticleSystem::ParticleSystem(Shader &shader, glm::vec3 position, glm::vec3 scale, float rotationAngle,
                               glm::vec3 rotationAxis, float mass, glm::vec3 velocity)
    : mGPassShader{&shader}, mParticleCount{0}, mMass{mass}, mMassInver{1.f / mass}, mVelocity{velocity},
      mGravity{glm::vec3(0.f, 0.f, 0.f)}, mDamping{0.995f}, mForceAccum{glm::vec3(0.f, 0.f, 0.f)} {
    mTransform.position = position;
    mTransform.scale = scale;

    // Compute the rotation matrix from the angle and axis given
    mTransform.orientation = glm::identity<glm::quat>();
    if (std::fabs(rotationAngle) > 1e-6f) {
        mTransform.orientation = glm::angleAxis(rotationAngle, rotationAxis);
    }

    mMaterial = std::make_unique<GLBase::Material>(shader, glm::vec3(0.f, 0.f, 0.f), 0.f);
}

// Set the geometry of a single particle
// Add geometrical object, and copy it to the list of elementary objects of
// the GLSandbox class
// Also sets the geometry of a single particle
void ParticleSystem::setParticleGeometry(std::unique_ptr<GLElemObject> particleObjectPtr, Shader *GPassShader) {
    // Add a GLParticleSystem object, with the given geometry
    mParticleSystemGL = std::make_unique<GLParticleSystem>(std::move(particleObjectPtr), GPassShader);

    // Copy a pointer to the list of particles
    mParticlesGL = mParticleSystemGL->getPointerToListOfParticles();
}

void ParticleSystem::setParticleGravity(glm::vec3 gravity) { mParticleGravity = gravity; }

void ParticleSystem::setVelocity(glm::vec3 velocity) { mVelocity = velocity; }
void ParticleSystem::setGravity(glm::vec3 gravity) { mGravity = gravity; }

void ParticleSystem::setDamping(float damping) { mDamping = damping; }

void ParticleSystem::setMass(float mass) {
    mMass = mass;
    mMassInver = 1.f / mass;
}
void ParticleSystem::setInvMass(float invMass) {
    mMassInver = invMass;
    if (invMass == 0.f)
        mMass = -1.f;
    else
        mMass = 1.f / invMass;
}

void ParticleSystem::addParticle(glm::vec3 velocity, glm::vec3 scale, float maxAge, std::unique_ptr<Material> material) {
    mParticlesGL->push_back(
        std::make_unique<GLParticle>(mTransform.position, mVelocity + velocity, scale, maxAge, std::move(material)));
    mParticleCount += 1;
}

GLGeometry::GLElemObject *ParticleSystem::getGeometry() { return mParticleSystemGL.get(); }

GLBase::Material *ParticleSystem::getMaterial() { return mMaterial.get(); }

bool ParticleSystem::hasGeometry() { return mParticleSystemGL && mMaterial; }

void ParticleSystem::integrate(float deltaTime) {
    if (mParticleCount < 100)
        addParticle(glm::vec3(-2.f + 4.f * Utils::getRandom0To1(), 15.f * Utils::getRandom0To1(),
                              -2.f + 4.f * Utils::getRandom0To1()),
                    0.5f * glm::vec3(1.f, 1.f, 1.f), 1.f + 5.f * Utils::getRandom0To1(),
                    std::make_unique<Material>(
                        *mGPassShader,
                        glm::vec3(Utils::getRandom0To1(), Utils::getRandom0To1(), Utils::getRandom0To1()), 0.5f, 1.f));

    if (mParticleCount == 0)
        return;

    for (std::list<std::unique_ptr<GLParticle>>::iterator particleItr = mParticlesGL->begin();
         particleItr != mParticlesGL->end(); ++particleItr) {
        GLParticle *particlePtr = (*particleItr).get();
        // If the age of the particle is too large, delete it
        if (particlePtr->age > particlePtr->maxAge) {
            // Delete the particle and move the iterator to the next one
            particleItr = mParticlesGL->erase(particleItr);
            mParticleCount -= 1;
            continue;
        }

        // Add deltaTime to the age of the particle
        particlePtr->age += deltaTime;

        // Compute acceleration, update linear velocity, and update position
        glm::vec3 resultingAcc = mParticleGravity;
        particlePtr->velocity += resultingAcc * deltaTime;
        particlePtr->velocity *= powf(mDamping, deltaTime);
        particlePtr->position += particlePtr->velocity * deltaTime;

        particlePtr->computeModelMatrix();
    }

    glm::vec3 resultingAcc = mGravity + mForceAccum * mMassInver;
    mVelocity += resultingAcc * deltaTime;
    // Drag on the velocity, so it does not increase due to numerical errors
    mVelocity *= powf(mDamping, deltaTime);

    mTransform.position += mVelocity * deltaTime;

    // Reset the net force and torque on the object
    mForceAccum = glm::vec3(0.f, 0.f, 0.f);
    // mTorqueAccum = glm::vec3( 0.f, 0.f, 0.f );
}

} // namespace Physics
