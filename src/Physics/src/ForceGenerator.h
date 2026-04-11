#ifndef FORCEGENERATOR_H
#define FORCEGENERATOR_H

#include "GLBase.h"
#include "interfaces.h"
#include "RigidBody.h"

using namespace GLBase;

namespace Physics {

// Virtual force generator
class ForceGenerator : public IValidatable {
public:
    virtual ~ForceGenerator() = default;

    // Calculate and apply the corresponding force to a RigidBody
    virtual void updateForce(RigidBody *rigidBody, float deltaTime) = 0;
};

// Gravity force generator
class GravityForceGenerator : public ForceGenerator {
public:
    GravityForceGenerator(const glm::vec3 &gravity);

    bool isValid() const override { return true; }

    virtual void updateForce(RigidBody *rigidBody, float deltaTime) override;

private:
    glm::vec3 mGravity;
};

// Drag force generator
class DragForceGenerator : public ForceGenerator {
public:
    DragForceGenerator(float k1, float k2);

    bool isValid() const override { return true; }

    virtual void updateForce(RigidBody *rigidBody, float deltaTime) override;

private:
    // Coefficients for velocity and velocity square
    float mK1;
    float mK2;
};

// Spring force generator
class SpringForceGenerator : public ForceGenerator {
public:
    SpringForceGenerator(RigidBody *other, float springConst, float dampingCoeff, float restLength);

    bool isValid() const override { return mOtherBody != nullptr; }

    virtual void updateForce(RigidBody *rigidBody, float deltaTime) override;

private:
    // The body at the other end of the spring
    RigidBody *mOtherBody;
    // Spring constant
    float mSpringConst;
    // Damping coefficient
    float mDampingCoeff;
    // Rest lenght
    float mRestLength;
};

// Bungee force generator
// This is a spring that only exerts a force when it is stretched
class BungeeForceGenerator : public ForceGenerator {
public:
    BungeeForceGenerator(RigidBody *other, float springConst, float restLength);

    bool isValid() const override { return mOtherBody != nullptr; }

    virtual void updateForce(RigidBody *rigidBody, float deltaTime) override;

private:
    // The body at the other end of the spring
    RigidBody *mOtherBody;
    // Spring constant
    float mSpringConst;
    // Rest lenght
    float mRestLength;
};

} // namespace Physics

#endif
