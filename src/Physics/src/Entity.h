#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

// #include "Physics.h"

// #include "ForceGenerator.h"
#include "GLBase.h"
#include "GLElemObject.h"
#include "GLGeometry.h"
#include "Colliders.h"
#include "RigidBody.h"
#include "Transform.h"

// Class for objects with collisions
class Entity {
public:
    Entity(glm::vec3 position, glm::vec3 scale, float rotationAngle,
           glm::vec3 rotationAxis);

    ~Entity();

    // Add geometrical object, and copy it to the list of elementary objects of
    // the GLSandbox class
    template <typename T, typename... Args> void addGeometry(Args &&...);

    template <typename T, typename... Args> void addCollider(Args &&...);

    template <typename T, typename... Args> void addRigidBody(Args &&...);

    template <typename T, typename... Args> void addMaterial(Args &&...);

    void setPosition(glm::vec3 position);

    void setScale(glm::vec3 scale);

    void setRotation(float angle, glm::vec3 axis);

    glm::vec3 getPosition();

    Physics::Collider *getCollider();
    Physics::RigidBody *getRigidBody();
    GLGeometry::GLElemObject *getGeometry();
    GLBase::Material *getMaterial();

    bool hasPhysics();
    bool hasGeometry();

    void integrate(float deltaTime);

protected:
    Physics::Transform mTransform;

    std::unique_ptr<Physics::Collider> mCollider;

    std::unique_ptr<Physics::RigidBody> mRigidBody;

    std::unique_ptr<GLGeometry::GLElemObject> mGeometryObject;

    std::unique_ptr<GLBase::Material> mMaterial;

    void updateModelMatrix();
};

#endif
