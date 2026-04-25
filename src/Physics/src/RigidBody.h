#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "GLBase.h"
#include "Transform.h"

namespace Physics {

// Class for objects with both collisions and dynamics
class RigidBody {
public:
    RigidBody(float mass, glm::vec3 velocity = {0.f, 0.f, 0.f}, glm::vec3 angularVelocity = {0.f, 0.f, 0.f});

    void setTransformPtr(Transform *transform);

    void setVelocity(glm::vec3 velocity);

    void setAngularVelocity(glm::vec3 angularVelocity);

    void setMass(float mass);
    void setInvMass(float invMass);

    void setDamping(float damping);

    float getMass();
    glm::vec3 getPosition();
    glm::vec3 getVelocity();

    bool hasInfiniteMass();

    // Add a force, expressed in world coordinates
    void addForce(const glm::vec3 &force);

    // TODO: Function addForceAtPoint
    // Add a force at a point, both expressed in world coordinates

    void integrate(float deltaTime);

protected:
    Transform *mTransform;

    float mMass;
    float mInvMass;

    glm::mat3 mInertiaTensorLocal;
    glm::mat3 mInvInertiaTensorLocal;
    glm::mat3 mInvInertiaTensorWorld;

    glm::vec3 mVelocity;
    // glm::vec3 mAcceleration;
    glm::vec3 mAngularVelocity;

    // Damping applied to linear motion, to ensure objects are not accelerated
    // due to numerical inaccuracies
    float mDamping;

    glm::vec3 mForceAccum;
    glm::vec3 mTorqueAccum;

    void clearAccumulators();

    void rotateInertiaTensor();
};

} // namespace Physics

#endif
