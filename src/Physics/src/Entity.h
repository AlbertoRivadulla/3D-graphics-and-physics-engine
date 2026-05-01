#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

#include "GLElemObject.h"
#include "Colliders.h"
#include "RigidBody.h"
#include "Transform.h"

// Class for objects with collisions
class Entity {
public:
    Entity(glm::vec3 position, glm::vec3 scale, float rotationAngle, glm::vec3 rotationAxis);

    ~Entity();

    // Add geometrical object, and copy it to the list of elementary objects of
    // the GLSandbox class
    template <typename T, typename... Args> void addGeometry(Args &&...args) {
        static_assert(std::is_base_of<GLGeometry::GLElemObject, T>::value, "T must derive from GLGeometry::GLElemObject");

        mGeometryObject = std::make_unique<T>(std::forward<Args>(args)...);

        updateModelMatrix();
    }
    void addGeometry(std::unique_ptr<GLGeometry::GLElemObject>);

    template <typename T, typename... Args> void addCollider(Args &&...args) {
        static_assert(std::is_base_of<Physics::Collider, T>::value, "T must derive from Physics::Collider");

        mCollider = std::make_unique<T>(std::forward<Args>(args)...);

        updateModelMatrix();
    }
    void addCollider(std::unique_ptr<Physics::Collider>);

    template <typename T, typename... Args> void addRigidBody(Args &&...args) {
        static_assert(std::is_base_of<Physics::RigidBody, T>::value, "T must derive from Physics::RigidBody");

        mRigidBody = std::make_unique<T>(std::forward<Args>(args)...);
        mRigidBody->setTransformPtr(&mTransform);
    }
    void addRigidBody(std::unique_ptr<Physics::RigidBody>);

    template <typename T, typename... Args> void addMaterial(Args &&...args) {
        static_assert(std::is_base_of<GLBase::Material, T>::value, "T must derive from GLBase::Material");

        mMaterial = std::make_unique<T>(std::forward<Args>(args)...);
    }
    void addMaterial(std::unique_ptr<GLBase::Material>);

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
