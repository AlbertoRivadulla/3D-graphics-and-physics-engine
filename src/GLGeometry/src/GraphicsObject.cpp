#include "GraphicsObject.h"
#include <variant>
#include <algorithm>

namespace GLGeometry {

GraphicsObject::GraphicsObject() : mData{ObjectWithMaterial()}, mGlobalModelMatrix(glm::mat4(1.f)) {}

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
        mData = std::vector<ObjectWithMaterial>{ObjectWithMaterial{object, material}};
    }

    return &std::get<std::vector<ObjectWithMaterial>>(mData).back();
}

void GraphicsObject::setModelMatrix(const glm::mat4 &transformMatrix) {
    mGlobalModelMatrix = transformMatrix;
}

bool GraphicsObject::hasGeometry() const {
    if (auto *item = std::get_if<ObjectWithMaterial>(&mData)) {
        return item->isInitialized();
    } else if (auto *vec = std::get_if<std::vector<ObjectWithMaterial>>(&mData)) {
        return !vec->empty() && std::all_of(vec->begin(), vec->end(),
                                            [](const ObjectWithMaterial &item) { return item.isInitialized(); });
    }

    return false;
}

void GraphicsObject::draw() const {
    std::visit([&](const auto &d){ drawImpl(d); }, mData);
}

void GraphicsObject::drawWithoutMaterial(GLBase::Shader *shader) const {
    std::visit([&](const auto &d){ drawWithoutMaterialImpl(d, shader); }, mData);
}

void GraphicsObject::drawImpl(const ObjectWithMaterial &object) const {
    object.configureShader(mGlobalModelMatrix);

    object.objectWithTransform.object->draw();
}

void GraphicsObject::drawImpl(const std::vector<ObjectWithMaterial> &object) const {
    // TODO: Implement drawing for composed objects

}

void GraphicsObject::drawWithoutMaterialImpl(const ObjectWithMaterial &object, GLBase::Shader *shader) const {
    shader->setMat4("model", mGlobalModelMatrix);

    object.objectWithTransform.object->draw();
}

void GraphicsObject::drawWithoutMaterialImpl(const std::vector<ObjectWithMaterial> &object, GLBase::Shader *shader) const {
    // TODO: Implement drawing for composed objects

}

} // namespace GLGeometry
