#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "GLBase.h"
#include "Transform.h"

namespace Physics {

// Class for objects with both collisions and dynamics
class RigidBody {
public:
    RigidBody(float mass, glm::vec3 velocity = {0.f, 0.f, 0.f});

    void setTransformPtr(Transform *transform);

    void setVelocity(glm::vec3 velocity);

    void setMass(float mass);
    void setInvMass(float invMass);

    void setDamping(float damping);

    float getMass();
    glm::vec3 getVelocity();

    bool hasInfiniteMass();

    void addForce(const glm::vec3 &force);

    void integrate(float deltaTime);

protected:
    Transform *mTransform;

    // Variables for dynamics
    float mMass;
    float mMassInver;
    glm::vec3 mVelocity;
    // glm::vec3 mAcceleration;
    // TODO: Add angular velocity

    // Damping applied to linear motion, to ensure objects are not accelerated
    // due to numerical inaccuracies
    float mDamping;

    glm::vec3 mForceAccum;
    // glm::vec3 mTorqueAccum;

    void clearAccumulators();

    // // --------------------
    //
    // glm::vec3 mAngularVelocity;
    // glm::vec3 mForce;
    // glm::vec3 mTorque;
};

} // namespace Physics

#endif
