#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

#include "GLElemObject.h"
#include "Colliders.h"
#include "RigidBody.h"
#include "Transform.h"
#include "interfaces.h"
#include "GraphicsObject.h"

// Class for objects with collisions
class Entity {
public:
    Entity(glm::vec3 position, glm::vec3 scale, float rotationAngle, glm::vec3 rotationAxis);

    ~Entity();

    template <typename T, typename... Args> Physics::Collider *addCollider(Args &&...args) {
        static_assert(std::is_base_of<Physics::Collider, T>::value, "T must derive from Physics::Collider");

        mCollider = std::make_unique<T>(std::forward<Args>(args)...);

        updateModelMatrix();

        return mCollider.get();
    }
    Physics::Collider *addCollider(std::unique_ptr<Physics::Collider>);

    template <typename T, typename... Args> Physics::RigidBody *addRigidBody(Args &&...args) {
        static_assert(std::is_base_of<Physics::RigidBody, T>::value, "T must derive from Physics::RigidBody");

        mRigidBody = std::make_unique<T>(std::forward<Args>(args)...);
        mRigidBody->setTransformPtr(&mTransform);

        return mRigidBody.get();
    }
    Physics::RigidBody *addRigidBody(std::unique_ptr<Physics::RigidBody>);

    void registerObserver(std::shared_ptr<IEntityObserver> observer);

    void setObject(GLGeometry::GLElemObject *geometry, GLBase::Material *material);
    void addObject(GLGeometry::GLElemObject *geometry, GLBase::Material *material);

    void setGeometry(GLGeometry::GLElemObject *geometry);

    void setMaterial(GLBase::Material *material);

    void setPosition(glm::vec3 position);

    void setScale(glm::vec3 scale);

    void setRotation(float angle, glm::vec3 axis);

    glm::vec3 getPosition() const;
    glm::quat getOrientation() const;

    GLGeometry::GraphicsObject *getGraphicsObject();
    Physics::Collider *getCollider();
    Physics::RigidBody *getRigidBody();
    Physics::RigidBody *getRigidBody() const;

    bool hasPhysics() const;
    bool hasGeometry() const;

    void integrate(float deltaTime);

protected:
    Physics::Transform mTransform;

    GLGeometry::GraphicsObject mGraphicsObject;

    std::unique_ptr<Physics::Collider> mCollider;

    std::unique_ptr<Physics::RigidBody> mRigidBody;

    std::vector<std::weak_ptr<IEntityObserver>> mObservers;

    void updateModelMatrix();

    void notifyObservers(float deltaTime);
};

#endif
