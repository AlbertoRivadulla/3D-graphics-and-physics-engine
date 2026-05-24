#include "RigidBody.h"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include "src/geometry.h"
#include "utils.h"

namespace Physics {

// Constructor
RigidBody::RigidBody(float mass, glm::mat3 inertiaTensor, glm::vec3 centerOfMass, glm::vec3 velocity,
                     glm::vec3 angularVelocity)
    : mTransform{nullptr}, mInvMass{1.f / mass}, mCenterOfMass{centerOfMass}, mInertiaTensorLocal{inertiaTensor},
      mInvInertiaTensorLocal(glm::inverse(inertiaTensor)), mVelocity{velocity}, mAngularVelocity(angularVelocity),
      mDamping{0.995f}, mForceAccum{glm::vec3(0.f, 0.f, 0.f)}, mTorqueAccum{glm::vec3(0.f, 0.f, 0.f)} // //
{
    setMass(mass);

    // Initialize the world inertia tensor with the local value
    mInvInertiaTensorWorld = mInvInertiaTensorLocal;
}

void RigidBody::setTransformPtr(Transform *transform) {
    mTransform = transform;
    rotateInertiaTensor();
}

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

void RigidBody::addComponent(float mass, glm::mat3 inertiaTensor, glm::vec3 centerOfMass, float rotationAngle,
                             glm::vec3 rotationAxis) {
    if (mComponents.empty()) {
        // Assume the initial inertia tensor is not rotated with respect to the Entity's system of coordinates
        mComponents.push_back(RigidBodyComponent{mMass, mInertiaTensorLocal, mCenterOfMass, glm::mat3(1.f)});
    }

    mComponents.push_back(RigidBodyComponent{mass, inertiaTensor, centerOfMass,
                                             Utils::computeRotationMatrix(rotationAngle, rotationAxis)});

    computeComposedProperties();
}

float RigidBody::getMass() const { return mMass; }
glm::vec3 RigidBody::getPosition() const { return mTransform->position; }
glm::vec3 RigidBody::getVelocity() const { return mVelocity; }
glm::vec3 RigidBody::getAngularVelocity() const { return mAngularVelocity; }

bool RigidBody::hasInfiniteMass() { return mInvMass < 0.f; }

void RigidBody::addForce(const glm::vec3 &force) { mForceAccum += force; }

void RigidBody::addForceLocal(const glm::vec3 &forceLocal) {
    glm::vec3 forceWorld = mTransform->orientation * forceLocal;

    addForce(forceWorld);
}

void RigidBody::addForceAtPoint(const glm::vec3 &force, const glm::vec3 &point) {
    // The force and point are both expressed in world coordinates
    mForceAccum += force;

    // Vector from the center of mass to the point of actuation
    glm::vec3 r = point - (mTransform->position + mTransform->orientation * mCenterOfMass);
    mTorqueAccum += glm::cross(r, force);
}

void RigidBody::addForceAtPointLocal(const glm::vec3 &forceLocal, const glm::vec3 &pointLocal) {
    // The force and point are both expressed in local coordinates
    glm::vec3 forceWorld = mTransform->orientation * forceLocal;
    glm::vec3 pointWorld = mTransform->orientation * pointLocal + mTransform->position;

    addForceAtPoint(forceWorld, pointWorld);
}

void RigidBody::addTorque(const glm::vec3 &torque) { mTorqueAccum += torque; }

void RigidBody::addTorqueLocal(const glm::vec3 &torqueLocal) {
    glm::vec3 torqueWorld = mTransform->orientation * torqueLocal;

    addTorque(torqueWorld);
}

// Set the accumulators to zero
void RigidBody::clearAccumulators() {
    mForceAccum = glm::vec3(0.f, 0.f, 0.f);
    mTorqueAccum = glm::vec3(0.f, 0.f, 0.f);
}

void RigidBody::rotateInertiaTensor() {
    mInvInertiaTensorWorld = Utils::rotateMatrix3(mInvInertiaTensorLocal, mTransform->orientation);
}

void RigidBody::computeComposedProperties() {
    // Compute the total mass and center of mass
    mMass = 0.f;
    mCenterOfMass = glm::vec3(0.f, 0.f, 0.f);
    for (const auto &comp : mComponents) {
        if (comp.mass < 0.f) {
            mMass = -1.f;
            mInvMass = 0.f;
            mInvInertiaTensorLocal = glm::mat3(0.f);
            mInvInertiaTensorWorld = glm::mat3(0.f);
            return;
        }

        mMass += comp.mass;
        mCenterOfMass += comp.mass * comp.centerOfMass;
    }
    mCenterOfMass /= mMass;
    mInvMass = 1.f / mMass;

    // Compute the inertia tensor
    mInertiaTensorLocal = glm::mat3(0.f);
    for (const auto &comp : mComponents) {
        // Compute the inertia tensor of the component with respect to the composed object's center of mass, using the
        // parallel axis theorem
        glm::vec3 r = comp.centerOfMass - mCenterOfMass;
        float r2 = glm::length2(r);
        mInertiaTensorLocal += Utils::rotateMatrix3(comp.localInertiaTensor, comp.rotation) +
                               comp.mass * (r2 * glm::mat3(1.f) - glm::outerProduct(r, r));
    }
    mInvInertiaTensorLocal = glm::inverse(mInertiaTensorLocal);

    rotateInertiaTensor();
}

// Integrate forward in time by the given duration
void RigidBody::integrate(float deltaTime) {
    // Compute acceleration from the force
    glm::vec3 resultingAcc = mForceAccum * mInvMass;
    mVelocity += resultingAcc * deltaTime;
    // Drag on the velocity, so it does not increase due to numerical errors
    mVelocity *= powf(mDamping, deltaTime);

    // Compute angular acceleration from the torque
    glm::vec3 angularAccel = mInvInertiaTensorWorld * mTorqueAccum;
    mAngularVelocity += angularAccel * deltaTime;
    mAngularVelocity *= powf(mDamping, deltaTime);

    // Integrate angular velocity
    // The derivative of a quaternion is: dq/dt = 0.5 * w * q, where w is the angular velocity as a pure quaternion
    glm::quat w(0.f, mAngularVelocity.x, mAngularVelocity.y, mAngularVelocity.z);
    mTransform->orientation += (w * mTransform->orientation) * 0.5f * deltaTime;
    // Prevent drift due to numeric errors
    mTransform->orientation = glm::normalize(mTransform->orientation);

    // mCenterOfMass is from tmTransform->position, in local coords
    // Integrate the center of mass in world coordinates
    glm::vec3 centerOfMassWorld = mTransform->position + mTransform->orientation * mCenterOfMass;
    centerOfMassWorld += mVelocity * deltaTime;

    // Back-derive the geometric origin from the new center of mass position
    mTransform->position = centerOfMassWorld - mTransform->orientation * mCenterOfMass;

    // Reset the net force and torque on the object
    clearAccumulators();

    rotateInertiaTensor();
}

} // namespace Physics
