#ifndef RIGIDBODY_H
#define RIGIDBODY_H

// #include "GLBase.h"
#include "Transform.h"
#include "InertiaTensors.h"

namespace Physics {

// Class for objects with both collisions and dynamics
class RigidBody {
public:
    RigidBody(float mass, glm::mat3 inertiaTensor, glm::vec3 velocity = {0.f, 0.f, 0.f},
              glm::vec3 angularVelocity = {0.f, 0.f, 0.f});

    void setTransformPtr(Transform *transform);

    void setVelocity(glm::vec3 velocity);

    void setAngularVelocity(glm::vec3 angularVelocity);

    void setMass(float mass);
    void setInvMass(float invMass);

    void setDamping(float damping);

    float getMass() const;
    glm::vec3 getPosition() const;
    glm::vec3 getVelocity() const;
    glm::vec3 getAngularVelocity() const;

    bool hasInfiniteMass();

    void addForce(const glm::vec3 &force);
    void addForceLocal(const glm::vec3 &forceLocal);

    void addForceAtPoint(const glm::vec3 &force, const glm::vec3 &point);
    void addForceAtPointLocal(const glm::vec3 &forceLocal, const glm::vec3 &pointLocal);

    void addTorque(const glm::vec3 &torque);
    void addTorqueLocal(const glm::vec3 &torqueLocal);

    void integrate(float deltaTime);

protected:
    Transform *mTransform;

    float mMass;
    float mInvMass;

    glm::mat3 mInvInertiaTensorLocal;
    glm::mat3 mInvInertiaTensorWorld;

    glm::vec3 mVelocity;
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
