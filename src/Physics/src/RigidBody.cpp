#include "RigidBody.h"
#include "glm/geometric.hpp"

using namespace GLBase;

namespace Physics {

// Constructor
RigidBody::RigidBody(float mass, glm::vec3 velocity, glm::vec3 angularVelocity)
    : mTransform{nullptr}, mInvMass{1.f / mass}, mVelocity{velocity}, mAngularVelocity(angularVelocity),
      mDamping{0.995f}, mForceAccum{glm::vec3(0.f, 0.f, 0.f)}, mTorqueAccum{glm::vec3(0.f, 0.f, 0.f)} // //
// mAngularVelocity { glm::vec3( 0.f, 0.f, 0.f ) },
// mForce { glm::vec3( 0.f, 0.f, 0.f ) },
// mTorque { glm::vec3( 0.f, 0.f, 0.f ) }
{
    setMass(mass);

    // TODO: Set inertia tensor and its inverse
}

void RigidBody::setTransformPtr(Transform *transform) { mTransform = transform; }

void RigidBody::setVelocity(glm::vec3 velocity) { mVelocity = velocity; }

void RigidBody::setAngularVelocity(glm::vec3 angularVelocity) { mAngularVelocity = angularVelocity; }

// Set velocity damping
void RigidBody::setDamping(float damping) { mDamping = damping; }

// Set mass
void RigidBody::setMass(float mass) {
    if (mass < 0.f) {
        mMass = -1.f;
        mInvMass = 0.f;
    } else {
        mMass = mass;
        mInvMass = 1.f / mass;
    }
}
void RigidBody::setInvMass(float invMass) {
    mInvMass = invMass;
    if (invMass == 0.f)
        mMass = -1.f;
    else
        mMass = 1.f / invMass;
}

// Getters
float RigidBody::getMass() { return mMass; }
glm::vec3 RigidBody::getPosition() { return mTransform->position; }
glm::vec3 RigidBody::getVelocity() { return mVelocity; }

// Check if it has infinite mass
bool RigidBody::hasInfiniteMass() { return mInvMass < 0.f; }

// Add a force
void RigidBody::addForce(const glm::vec3 &force) { mForceAccum += force; }

// Set the accumulators to zero
void RigidBody::clearAccumulators() {
    mForceAccum = glm::vec3(0.f, 0.f, 0.f);
    mTorqueAccum = glm::vec3(0.f, 0.f, 0.f);
}

void RigidBody::rotateInertiaTensor() {
    glm::mat3 R = glm::toMat3(mTransform->orientation);

    // I_world_inv = R * I_local_inv * R^T
    mInvInertiaTensorWorld = R * mInvInertiaTensorLocal * glm::transpose(R);
}

// Integrate forward in time by the given duration
void RigidBody::integrate(float deltaTime) {
    // Compute acceleration from the force
    glm::vec3 resultingAcc = mForceAccum * mInvMass;
    // Update linear velocity
    mVelocity += resultingAcc * deltaTime;
    // Drag on the velocity, so it does not increase due to numerical errors
    mVelocity *= powf(mDamping, deltaTime);

    // Update the position
    mTransform->position += mVelocity * deltaTime;

    // TODO: Handle torque
    // glm::vec3 angularAccel = mInvInertiaTensorWorld * mTorqueAccum;
    // mAngularVelocity += angularAccel * deltaTime;

    // Integrate angular velocity
    // The derivative of a quaternion is: dq/dt = 0.5 * w * q, where w is the angular velocity as a pure quaternion
    glm::quat w(0.f, mAngularVelocity.x, mAngularVelocity.y, mAngularVelocity.z);
    mTransform->orientation += (w * mTransform->orientation) * 0.5f * deltaTime;
    // Prevent drift due to numeric errors
    mTransform->orientation = glm::normalize(mTransform->orientation);

    // Reset the net force and torque on the object
    clearAccumulators();

    // TODO:
    // Rotate the inertia tensor in world space
}

} // namespace Physics
