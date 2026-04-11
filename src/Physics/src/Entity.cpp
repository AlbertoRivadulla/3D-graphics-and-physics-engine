#include "Entity.h"

Entity::Entity(glm::vec3 position, glm::vec3 scale, float rotationAngle, glm::vec3 rotationAxis) {
    mTransform.position = position;
    mTransform.scale = scale;

    // Compute the rotation matrix from the angle and axis given
    mTransform.rotationMatrix = glm::mat4(1.f);
    if (rotationAngle != 0.)
        mTransform.rotationMatrix =
            glm::rotate(mTransform.rotationMatrix, glm::radians(rotationAngle), glm::normalize(rotationAxis));
}

Entity::~Entity() {}

void Entity::addGeometry(std::unique_ptr<GLElemObject> geometry) {
    mGeometryObject = std::move(geometry);

    updateModelMatrix();
}

void Entity::addCollider(std::unique_ptr<Physics::Collider> collider) {
    mCollider = std::move(collider);

    updateModelMatrix();
}

void Entity::addRigidBody(std::unique_ptr<Physics::RigidBody> rigidBody) {
    mRigidBody = std::move(rigidBody);
    mRigidBody->setTransformPtr(&mTransform);
}

void Entity::addMaterial(std::unique_ptr<GLBase::Material> material) { mMaterial = std::move(material); }

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
    mTransform.rotationMatrix = glm::mat4(1.f);
    if (angle != 0.)
        mTransform.rotationMatrix = glm::rotate(mTransform.rotationMatrix, glm::radians(angle), glm::normalize(axis));

    updateModelMatrix();
}

glm::vec3 Entity::getPosition() { return mTransform.position; }

Physics::Collider *Entity::getCollider() { return mCollider.get(); }
Physics::RigidBody *Entity::getRigidBody() { return mRigidBody.get(); }
GLGeometry::GLElemObject *Entity::getGeometry() { return mGeometryObject.get(); }
GLBase::Material *Entity::getMaterial() { return mMaterial.get(); }

bool Entity::hasPhysics() { return mCollider || mRigidBody; }

bool Entity::hasGeometry() { return mGeometryObject && mMaterial; }

void Entity::integrate(float deltaTime) {
    if (mRigidBody) {
        mRigidBody->integrate(deltaTime);

        updateModelMatrix();
    }
}

void Entity::updateModelMatrix() {
    mTransform.modelMatrix = glm::mat4(1.f);
    mTransform.modelMatrix = glm::translate(mTransform.modelMatrix, mTransform.position);
    mTransform.modelMatrix = mTransform.modelMatrix * mTransform.rotationMatrix;
    mTransform.modelMatrix = glm::scale(mTransform.modelMatrix, mTransform.scale);

    if (mCollider) {
        mCollider->moveCollider(mTransform.modelMatrix);
    }

    if (mGeometryObject) {
        mGeometryObject->setModelMatrix(mTransform.modelMatrix);
        // TODO: I think this model matrix is the same for both objects. If this
        // fails, uncomment the next line
        // mGeometryObject->setModelMatrix(mPosition, mRotationMatrix, mScale);
    }
}
