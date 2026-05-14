#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

#include "GLElemObject.h"
#include "Colliders.h"
#include "RigidBody.h"
#include "Transform.h"
#include "interfaces.h"

// Class for objects with collisions
class Entity {
public:
    Entity(glm::vec3 position, glm::vec3 scale, float rotationAngle, glm::vec3 rotationAxis);

    ~Entity();

    // Add geometrical object, and copy it to the list of elementary objects of
    // the GLSandbox class
    template <typename T, typename... Args> GLGeometry::GLElemObject* addGeometry(Args &&...args) {
        static_assert(std::is_base_of<GLGeometry::GLElemObject, T>::value, "T must derive from GLGeometry::GLElemObject");

        mGeometryObject = std::make_unique<T>(std::forward<Args>(args)...);

        updateModelMatrix();

        return mGeometryObject.get();
    }
    GLGeometry::GLElemObject* addGeometry(std::unique_ptr<GLGeometry::GLElemObject>);

    template <typename T, typename... Args> Physics::Collider *addCollider(Args &&...args) {
        static_assert(std::is_base_of<Physics::Collider, T>::value, "T must derive from Physics::Collider");

        mCollider = std::make_unique<T>(std::forward<Args>(args)...);

        updateModelMatrix();

        return mCollider.get();
    }
    Physics::Collider *addCollider(std::unique_ptr<Physics::Collider>);

    template <typename T, typename... Args> Physics::RigidBody* addRigidBody(Args &&...args) {
        static_assert(std::is_base_of<Physics::RigidBody, T>::value, "T must derive from Physics::RigidBody");

        mRigidBody = std::make_unique<T>(std::forward<Args>(args)...);
        mRigidBody->setTransformPtr(&mTransform);

        return mRigidBody.get();
    }
    Physics::RigidBody* addRigidBody(std::unique_ptr<Physics::RigidBody>);

    template <typename T, typename... Args> void addMaterial(Args &&...args) {
        static_assert(std::is_base_of<GLBase::Material, T>::value, "T must derive from GLBase::Material");

        mMaterial = std::make_unique<T>(std::forward<Args>(args)...);
    }
    void addMaterial(std::unique_ptr<GLBase::Material>);

    void registerObserver(std::shared_ptr<IEntityObserver> observer);

    void setPosition(glm::vec3 position);

    void setScale(glm::vec3 scale);

    void setRotation(float angle, glm::vec3 axis);

    glm::vec3 getPosition() const;
    glm::quat getOrientation() const;

    Physics::Collider *getCollider();
    Physics::RigidBody *getRigidBody();
    Physics::RigidBody *getRigidBody() const;
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

    std::vector<std::weak_ptr<IEntityObserver>> mObservers;

    void updateModelMatrix();

    void notifyObservers(float deltaTime);
};

#endif
