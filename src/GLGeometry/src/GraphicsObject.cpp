#include "GraphicsObject.h"
#include "src/geometry.h"
#include <variant>
#include <algorithm>

namespace GLGeometry {

GraphicsObject::GraphicsObject() : mData{ObjectWithMaterial()}, mGlobalModelMatrix(glm::mat4(1.f)) {}

bool GraphicsObject::hasGeometry() const {
    if (auto *item = std::get_if<ObjectWithMaterial>(&mData)) {
        return item->isInitialized();
    } else if (auto *vec = std::get_if<std::vector<ObjectWithMaterial>>(&mData)) {
        return !vec->empty() && std::all_of(vec->begin(), vec->end(),
                                            [](const ObjectWithMaterial &item) { return item.isInitialized(); });
    }

    return false;
}

ObjectWithMaterial *GraphicsObject::setObject(GLGeometry::GLElemObject *object, GLBase::Material *material) {
    // In both cases, mData should contain the single object given
    mData = ObjectWithMaterial{object, material};

    return &std::get<ObjectWithMaterial>(mData);
}

ObjectWithMaterial *GraphicsObject::addObject(GLElemObject *object, GLBase::Material *material) {
    if (std::holds_alternative<ObjectWithMaterial>(mData)) {
        std::vector<ObjectWithMaterial> vec;

        auto item = std::move(std::get<ObjectWithMaterial>(mData));
        if (item.isInitialized()) {
            vec.push_back(std::move(item));
        }

        vec.push_back(ObjectWithMaterial{object, material});
        mData = std::move(vec);

    } else {
        std::get<std::vector<ObjectWithMaterial>>(mData).push_back(ObjectWithMaterial{object, material});
    }

    return &std::get<std::vector<ObjectWithMaterial>>(mData).back();
}

void GraphicsObject::setGeometry(GLGeometry::GLElemObject *geometry) {
    assert(std::holds_alternative<ObjectWithMaterial>(mData));

    std::get<ObjectWithMaterial>(mData).objectWithTransform.object = geometry;
}
void GraphicsObject::setGeometry(size_t index, GLGeometry::GLElemObject *geometry) {
    assert(std::holds_alternative<std::vector<ObjectWithMaterial>>(mData));

    auto &vec = std::get<std::vector<ObjectWithMaterial>>(mData);
    vec[index].objectWithTransform.object = geometry;
}

void GraphicsObject::setGlobalModelMatrix(const glm::mat4 &transformMatrix) {
    mGlobalModelMatrix = transformMatrix;
}

void GraphicsObject::setGlobalModelMatrix(const glm::vec3 &translation, const float &rotationAngle,
                                          const glm::vec3 &rotationAxis, const glm::vec3 &scale) {
    mGlobalModelMatrix = Utils::computeModelMatrix(translation, rotationAngle, rotationAxis, scale);
}
void GraphicsObject::setGlobalModelMatrix(const glm::vec3 &translation, const glm::mat4 &rotationMatrix,
                                          const glm::vec3 &scale) {
    mGlobalModelMatrix = Utils::computeModelMatrix(translation, rotationMatrix, scale);
}

void GraphicsObject::setMaterial(GLBase::Material *material) {
    assert(std::holds_alternative<ObjectWithMaterial>(mData));

    std::get<ObjectWithMaterial>(mData).material = material;
}

void GraphicsObject::setMaterial(size_t index, GLBase::Material *material) {
    assert(std::holds_alternative<std::vector<ObjectWithMaterial>>(mData));

    auto &vec = std::get<std::vector<ObjectWithMaterial>>(mData);
    vec[index].material = material;
}

void GraphicsObject::draw() const {
    std::visit([&](const auto &d) { drawImpl(d); }, mData);
}

void GraphicsObject::drawWithoutMaterial(GLBase::Shader *shader) const {
    std::visit([&](const auto &d) { drawWithoutMaterialImpl(d, shader); }, mData);
}

void GraphicsObject::drawImpl(const ObjectWithMaterial &object) const {
    object.configureShader(mGlobalModelMatrix);

    object.objectWithTransform.object->draw();
}

void GraphicsObject::drawImpl(const std::vector<ObjectWithMaterial> &objects) const {
    for (auto &component : objects) {
        component.configureShader(mGlobalModelMatrix * component.objectWithTransform.getModelMatrix());

        component.objectWithTransform.object->draw();
    }
}

void GraphicsObject::drawWithoutMaterialImpl(const ObjectWithMaterial &object, GLBase::Shader *shader) const {
    shader->setMat4("model", mGlobalModelMatrix);

    object.objectWithTransform.object->draw();
}

void GraphicsObject::drawWithoutMaterialImpl(const std::vector<ObjectWithMaterial> &objects,
                                             GLBase::Shader *shader) const {
    for (auto &component : objects) {
        // shader->setMat4("model", component.objectWithTransform.getModelMatrix() * mGlobalModelMatrix);
        shader->setMat4("model", mGlobalModelMatrix * component.objectWithTransform.getModelMatrix());

        component.objectWithTransform.object->draw();
    }
}

} // namespace GLGeometry
