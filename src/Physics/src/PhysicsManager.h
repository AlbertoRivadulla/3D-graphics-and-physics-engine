#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include <vector>
#include "ForceGenerator.h"
#include "Entity.h"
#include "Terrain.h"
#include "ParticleSystem.h"

namespace Physics {

// Registry for the forces that apply to each body in the world
class BodyForceRegistry {
protected:
    // One force and the body it is applied to
    struct BodyForceRegistration {
        RigidBody *rigidBody;
        ForceGenerator *forceGenerator;
        BodyForceRegistration(RigidBody *body, ForceGenerator *force) {
            rigidBody = body;
            forceGenerator = force;
        }
    };

    // List of all registrations
    // I think this works the same for a vector or a linked list
    // typedef std::vector<BodyForceRegistration> Registry;
    typedef std::list<BodyForceRegistration> Registry;
    Registry mRegistrations;

public:
    // Register a pair body-force
    void registerBodyForce(RigidBody *body, ForceGenerator *force);

    // Remove a pair body-force
    // If the pair is not registrated, this will not do anything
    void removeBodyForce(RigidBody *body, ForceGenerator *force);

    void clear();

    // Call the force generators to update the forces on the particles
    void applyForces(float deltaTime);
};

class PhysicsManager {
public:
    PhysicsManager();

    ~PhysicsManager();

    void registerEntity(Entity *entity);

    void registerTerrain(Terrain *terrain);

    void registerParticleSystem(ParticleSystem *particleSystem);

    template <typename T, typename... Args> ForceGenerator *addGravity(Args &&...args) {
        static_assert(std::is_base_of<ForceGenerator, T>::value, "T must derive from Physics::ForceGenerator");

        mGravity = std::make_unique<T>(std::forward<Args>(args)...);

        return mGravity.get();
    }

    ForceGenerator *addGravity(std::unique_ptr<ForceGenerator> gravity);

    template <typename T, typename... Args> ForceGenerator *addForce(Args &&...args) {
        static_assert(std::is_base_of<ForceGenerator, T>::value, "T must derive from Physics::ForceGenerator");

        auto force = std::make_unique<T>(std::forward<Args>(args)...);
        ForceGenerator *raw = force.get();
        mForceGenerators.push_back(std::move(force));

        return raw;
    }
    ForceGenerator *addForce(std::unique_ptr<ForceGenerator> force);

    void registerBodyGravity(Entity *entity);
    void registerBodyGravity(RigidBody *body);

    // Register a pair body-force
    void registerBodyForce(Entity *entity, ForceGenerator *force);
    void registerBodyForce(RigidBody *body, ForceGenerator *force);

    // Remove a pair body-force
    void removeBodyForce(RigidBody *body, ForceGenerator *force);

    // Update the objects in the current frame
    void step(float deltaTime);

private:
    std::vector<Entity *> mEntitiesWithDynamics;
    std::vector<Entity *> mEntitiesWithCollisions;

    Terrain *mTerrain;

    std::vector<ParticleSystem *> mParticleSystems;

    std::vector<std::unique_ptr<ForceGenerator>> mForceGenerators;
    std::unique_ptr<ForceGenerator> mGravity;

    // Registry of the forces applied to each body
    BodyForceRegistry mBodyForceRegistry;

    // NOTE: This is here only for debugging
    // Used to slow down simulations
    int mCounter;

    bool checkForceIsRegistered(ForceGenerator *force);
};

} // namespace Physics

#endif
