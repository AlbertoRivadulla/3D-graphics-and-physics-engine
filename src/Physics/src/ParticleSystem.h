#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "GLBase.h"
#include "GLGeometry.h"
#include "Transform.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics {
// Class for the particle system
// TODO: Refactor this. Maybe it should derive from the Entity class
class ParticleSystem {
public:
    ParticleSystem(Shader &shader, glm::vec3 position, glm::vec3 scale,
                   float rotationAngle, glm::vec3 rotationAxis, float mass,
                   glm::vec3 velocity = {0.f, 0.f, 0.f});

    // Set the geometry of a single particle
    // Add geometrical object, and copy it to the list of elementary objects of
    // the GLSandbox class
    // Also sets the geometry of a single particle
    void setParticleGeometry(std::unique_ptr<GLElemObject> particleObjectPtr,
                             Shader *GPassShader);

    void setParticleGravity(glm::vec3 gravity);

    void setVelocity(glm::vec3 velocity);
    void setGravity(glm::vec3 gravity);

    void setMass(float mass);
    void setInvMass(float invMass);

    void setDamping(float damping);

    void addParticle(glm::vec3 velocity, glm::vec3 scale, float maxAge,
                     Material *material);

    GLGeometry::GLElemObject *getGeometry();
    GLBase::Material *getMaterial();

    bool hasGeometry();

    void integrate(float deltaTime);

private:
    std::unique_ptr<GLBase::Material> mMaterial;

    // Particle system object for the graphics
    std::unique_ptr<GLParticleSystem> mParticleSystemGL;

    // Pointer to the list of particles in mParticleSystemGL
    std::list<std::unique_ptr<GLParticle>> *mParticlesGL;

    // Pointer to the shader for the G pass
    Shader *mGPassShader;
    // Gravity of particles
    glm::vec3 mParticleGravity;

    int mParticleCount;

    // Variables for dynamics
    Physics::Transform mTransform;
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

    // NOTE: If I want to add a collider I will have to add also a model matrix etc.
    // At this point it would make sense to make this class derived of Entity
};
} // namespace Physics

#endif
