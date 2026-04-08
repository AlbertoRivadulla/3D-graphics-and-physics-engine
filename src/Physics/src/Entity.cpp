#include "Entity.h"

Entity::Entity(glm::vec3 position, glm::vec3 scale, float rotationAngle,
               glm::vec3 rotationAxis) {
    mTransform.position = position;
    mTransform.scale = scale;

    // Compute the rotation matrix from the angle and axis given
    mTransform.rotationMatrix = glm::mat4(1.f);
    if (rotationAngle != 0.)
        mTransform.rotationMatrix =
            glm::rotate(mTransform.rotationMatrix, glm::radians(rotationAngle),
                        glm::normalize(rotationAxis));
}

Entity::~Entity() {}

// Add geometrical object, and copy it to the list of elementary objects of
// the GLSandbox class
template <typename T, typename... Args>
void Entity::addGeometry(Args &&...args) {
    static_assert(std::is_base_of<GLElemObject, T>::value,
                  "T must derive from GLElemObject");

    mGeometryObject = std::make_unique<T>(std::forward<Args>(args)...);

    updateModelMatrix();

    // TODO: Make sure this is passed to the world manager object
}

template <typename T, typename... Args>
void Entity::addCollider(Args &&...args) {
    static_assert(std::is_base_of<Physics::Collider, T>::value,
                  "T must derive from Physics::Collider");

    mCollider = std::make_unique<T>(std::forward<Args>(args)...);

    updateModelMatrix();
}

template <typename T, typename... Args>
void Entity::addRigidBody(Args &&...args) {
    static_assert(std::is_base_of<Physics::RigidBody, T>::value,
                  "T must derive from Physics::RigidBody");

    mRigidBody = std::make_unique<T>(std::forward<Args>(args)...);
    mRigidBody->setTransformPtr(&mTransform);
}

template <typename T, typename... Args>
void Entity::addMaterial(Args &&...args) {
    static_assert(std::is_base_of<GLBase::Material, T>::value,
                  "T must derive from GLBase::Material");

    mMaterial = std::make_unique<T>(std::forward<Args>(args)...);
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
    mTransform.rotationMatrix = glm::mat4(1.f);
    if (angle != 0.)
        mTransform.rotationMatrix =
            glm::rotate(mTransform.rotationMatrix, glm::radians(angle),
                        glm::normalize(axis));

    updateModelMatrix();
}

glm::vec3 Entity::getPosition() { return mTransform.position; }

Physics::Collider *Entity::getCollider() {
    return mCollider.get();
}
Physics::RigidBody *Entity::getRigidBody() {
    return mRigidBody.get();
}
GLGeometry::GLElemObject *Entity::getGeometry() {
    return mGeometryObject.get();
}
GLBase::Material *Entity::getMaterial() {
    return mMaterial.get();
}

bool Entity::hasPhysics() {
    return mCollider || mRigidBody;
}

bool Entity::hasGeometry() {
    return mGeometryObject && mMaterial;
}

void Entity::integrate(float deltaTime) {
    if (mRigidBody) {
        mRigidBody->integrate(deltaTime);

        updateModelMatrix();
    }
}

void Entity::updateModelMatrix() {
    mTransform.modelMatrix = glm::mat4(1.f);
    mTransform.modelMatrix =
        glm::translate(mTransform.modelMatrix, mTransform.position);
    mTransform.modelMatrix = mTransform.modelMatrix * mTransform.rotationMatrix;
    mTransform.modelMatrix =
        glm::scale(mTransform.modelMatrix, mTransform.scale);


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
