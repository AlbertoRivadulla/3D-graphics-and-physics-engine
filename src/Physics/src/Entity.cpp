#include "Entity.h"
#include "glm/gtx/quaternion.hpp"

Entity::Entity(glm::vec3 position, glm::vec3 scale, float rotationAngle, glm::vec3 rotationAxis) {
    mTransform.position = position;
    mTransform.scale = scale;

    // Compute the rotation matrix from the angle and axis given
    mTransform.orientation = glm::identity<glm::quat>();
    if (std::fabs(rotationAngle) > 1e-6f) {
        mTransform.orientation = glm::angleAxis(rotationAngle, rotationAxis);
    }
}

Entity::~Entity() {}

Physics::Collider *Entity::addCollider(std::unique_ptr<Physics::Collider> collider) {
    mCollider = std::move(collider);

    updateModelMatrix();

    return mCollider.get();
}

Physics::RigidBody *Entity::addRigidBody(std::unique_ptr<Physics::RigidBody> rigidBody) {
    mRigidBody = std::move(rigidBody);
    mRigidBody->setTransformPtr(&mTransform);

    return mRigidBody.get();
}

void Entity::registerObserver(std::shared_ptr<IEntityObserver> observer) {
    // This implicitly converts the shared_ptr to weak_ptr
    mObservers.push_back(observer);
}

void Entity::setPosition(glm::vec3 position) {
    mTransform.position = position;

    updateModelMatrix();
}

void Entity::setScale(glm::vec3 scale) {
    mTransform.scale = scale;

    updateModelMatrix();
}

void Entity::setRotation(float angle, glm::vec3 axis) {
    // Compute the rotation matrix from the angle and axis given
    mTransform.orientation = glm::identity<glm::quat>();
    if (std::fabs(angle) > 1e-6f) {
        mTransform.orientation = glm::angleAxis(angle, axis);
    }

    updateModelMatrix();
}

glm::vec3 Entity::getPosition() const { return mTransform.position; }
glm::quat Entity::getOrientation() const { return mTransform.orientation; }

GLGeometry::GraphicsObject *Entity::getGraphicsObject() { return &mGraphicsObject; }
Physics::Collider *Entity::getCollider() { return mCollider.get(); }
Physics::RigidBody *Entity::getRigidBody() { return mRigidBody.get(); }
Physics::RigidBody *Entity::getRigidBody() const { return mRigidBody.get(); }

bool Entity::hasPhysics() const { return mCollider || mRigidBody; }

bool Entity::hasGeometry() const { return mGraphicsObject.hasGeometry(); }

void Entity::integrate(float deltaTime) {
    if (mRigidBody) {
        mRigidBody->integrate(deltaTime);

        updateModelMatrix();
    }

    notifyObservers(deltaTime);
}

void Entity::updateModelMatrix() {
    mTransform.modelMatrix = glm::mat4(1.f);
    mTransform.modelMatrix = glm::translate(mTransform.modelMatrix, mTransform.position);
    mTransform.modelMatrix = mTransform.modelMatrix * glm::toMat4(mTransform.orientation);
    mTransform.modelMatrix = glm::scale(mTransform.modelMatrix, mTransform.scale);

    if (mCollider) {
        mCollider->moveCollider(mTransform.modelMatrix);
    }

    if (mGraphicsObject.hasGeometry()) {
        mGraphicsObject.setModelMatrix(mTransform.modelMatrix);
    }
}

void Entity::notifyObservers(float deltaTime) {
    if (mObservers.empty())
        return;

    for (auto it = mObservers.begin(); it != mObservers.end();) {
        if (auto observer = it->lock()) { // This converts the weak_ptr into a shared_ptr
            observer->onEntityUpdated(*this, deltaTime);
            ++it;
        } else {
            // If it->lock() failed, the shared_ptr is no longer valid
            it = mObservers.erase(it);
        }
    }

    // NOTE: A class Vehicle : public Entity that has a vector of IEntityObserverT<Vehicle> should override this method with:
    //
    // void Vehicle::notifyObservers(float deltaTime) {
    // Entity::notifyObservers(deltaTime);
    //
    // if (mVehicleObservers.empty()) return;
    //
    // for (auto it = mVehicleObservers.begin(); it != mVehicleObservers.end(); ) {
    //     if (auto observer = it->lock()) {
    //         observer->onEntityUpdated(*this, deltaTime);
    //         ++it;
    //     } else {
    //         it = mVehicleObservers.erase(it);  // auto-cleanup expired observers
    //     }
    // }
}
