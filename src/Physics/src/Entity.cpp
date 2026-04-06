#include "Entity.h"
#include "utils.h"

using namespace GLGeometry;
using namespace GLBase;

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

    computeModelMatrix();

    // TODO: Make sure this is passed to the world manager object
}

template <typename T, typename... Args>
void Entity::addCollider(Args &&...args) {
    static_assert(std::is_base_of<Physics::Collider, T>::value,
                  "T must derive from Collider");

    mCollider = std::make_unique<T>(std::forward<Args>(args)...);

    // TODO: This could be done without copying data
    // Pass the transformation matrix to the collider
    mCollider->moveCollider(mTransform.modelMatrix);
}

template <typename T, typename... Args>
void Entity::addRigidBody(Args &&...args) {
    static_assert(std::is_base_of<Physics::RigidBody, T>::value,
                  "T must derive from RigidBody");

    mRigidBody = std::make_unique<T>(std::forward<Args>(args)...);

    // TODO: Set pointers to transform etc
}

template <typename T, typename... Args>
void Entity::addMaterial(Args &&...args) {
    static_assert(std::is_base_of<Material, T>::value,
                  "T must derive from Material");

    mMaterial = std::make_unique<T>(std::forward<Args>(args)...);
}

void Entity::setPosition(glm::vec3 position) {
    mTransform.position = position;

    // Update the model matrix of the geometry object
    computeModelMatrix();
}

void Entity::setScale(glm::vec3 scale) {
    mTransform.scale = scale;

    // Update the model matrix
    computeModelMatrix();
}

void Entity::setRotation(float angle, glm::vec3 axis) {
    // Compute the rotation matrix from the angle and axis given
    mTransform.rotationMatrix = glm::mat4(1.f);
    if (angle != 0.)
        mTransform.rotationMatrix =
            glm::rotate(mTransform.rotationMatrix, glm::radians(angle),
                        glm::normalize(axis));

    // Update the model matrix
    computeModelMatrix();
}

glm::vec3 Entity::getPosition() { return mTransform.position; }

void Entity::computeModelMatrix() {
    mTransform.modelMatrix = glm::mat4(1.f);
    mTransform.modelMatrix =
        glm::translate(mTransform.modelMatrix, mTransform.position);
    mTransform.modelMatrix = mTransform.modelMatrix * mTransform.rotationMatrix;
    mTransform.modelMatrix =
        glm::scale(mTransform.modelMatrix, mTransform.scale);

    mGeometryObject->setModelMatrix(mTransform.modelMatrix);
    // TODO: I think this model matrix is the same for both objects. If this
    // fails, uncomment the next line mGeometryObject->setModelMatrix(mPosition,
    // mRotationMatrix, mScale);
}

void Entity::draw() {
    // TODO: This should not be here.
    // Instead, the manager should draw only the entities that have been
    // registered with a material and a geometry object.

    mMaterial->configShader(mGeometryObject->getModelMatrix());

    mGeometryObject->draw();
}
