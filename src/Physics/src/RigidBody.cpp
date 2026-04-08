#include "RigidBody.h"

using namespace GLBase;

namespace Physics {

// Constructor
RigidBody::RigidBody(float mass, glm::vec3 velocity)
    : mTransform{nullptr}, mMassInver{1.f / mass}, mVelocity{velocity},
      // mAcceleration { glm::vec3( 0.f, 0.f, 0.f ) },
      mDamping{0.995f}, mForceAccum{glm::vec3(0.f, 0.f, 0.f)}
// mTorqueAccum { glm::vec3( 0.f, 0.f, 0.f ) },
// //
// mAngularVelocity { glm::vec3( 0.f, 0.f, 0.f ) },
// mForce { glm::vec3( 0.f, 0.f, 0.f ) },
// mTorque { glm::vec3( 0.f, 0.f, 0.f ) }
{
    setMass(mass);
}

void RigidBody::setTransformPtr(Transform *transform) {
    mTransform = transform;
}

// Set velocity and acceleration
void RigidBody::setVelocity(glm::vec3 velocity) { mVelocity = velocity; }

// Set velocity damping
void RigidBody::setDamping(float damping) { mDamping = damping; }

// Set mass
void RigidBody::setMass(float mass) {
    if (mass < 0.f) {
        mMass = -1.f;
        mMassInver = 0.f;
    } else {
        mMass = mass;
        mMassInver = 1.f / mass;
    }
}
void RigidBody::setInvMass(float invMass) {
    mMassInver = invMass;
    if (invMass == 0.f)
        mMass = -1.f;
    else
        mMass = 1.f / invMass;
}

// Getters
float RigidBody::getMass() { return mMass; }
glm::vec3 RigidBody::getVelocity() { return mVelocity; }

// Check if it has infinite mass
bool RigidBody::hasInfiniteMass() { return mMassInver < 0.f; }

// Add a force
void RigidBody::addForce(const glm::vec3 &force) { mForceAccum += force; }

// Set the accumulators to zero
void RigidBody::clearAccumulators() {
    mForceAccum = glm::vec3(0.f, 0.f, 0.f);
    // mTorqueAccum = glm::vec3( 0.f, 0.f, 0.f );
}

// Integrate forward in time by the given duration
void RigidBody::integrate(float deltaTime) {
    // Compute acceleration from the force
    glm::vec3 resultingAcc = mForceAccum * mMassInver;
    // Update linear velocity
    mVelocity += resultingAcc * deltaTime;
    // Drag on the velocity, so it does not increase due to numerical errors
    mVelocity *= powf(mDamping, deltaTime);

    // Update the position
    mTransform->position += mVelocity * deltaTime;

    // TODO: Handle rotations also here

    // Reset the net force and torque on the object
    clearAccumulators();
}

} // namespace Physics
