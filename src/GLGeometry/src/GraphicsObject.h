#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include <variant>
#include <vector>
#include "shader.h"
#include "GLElemObject.h"

namespace GLGeometry {

struct ObjectWithMaterial {
    GLObjectWithTransform objectWithTransform;
    GLBase::Material *material = nullptr;

    ObjectWithMaterial() = default;

    ObjectWithMaterial(GLElemObject *objectPtr, GLBase::Material *materialPtr = nullptr) {
        objectWithTransform.object = objectPtr;
        material = materialPtr;
    }

    bool isInitialized() const { return objectWithTransform.object != nullptr && material != nullptr; }

    void configureShader(const glm::mat4 &modelMatrix) const {
        material->configShaderWithModelMatrix(modelMatrix);
    }
};

class GraphicsObject {
public:
    GraphicsObject();

    ObjectWithMaterial *setObject(GLGeometry::GLElemObject *object, GLBase::Material *material = nullptr);

    ObjectWithMaterial *addObject(GLGeometry::GLElemObject *object, GLBase::Material *material = nullptr);

    void setModelMatrix(const glm::mat4 &transformMatrix);

    bool hasGeometry() const;

    // TODO: Methods to set material and transform matrix
    // Also methods for setting the material or matrix of one of the elements, with its index [?]

    void draw() const;

    void drawWithoutMaterial(GLBase::Shader *shader) const;

private:
    std::variant<ObjectWithMaterial, std::vector<ObjectWithMaterial>> mData;

    glm::mat4 mGlobalModelMatrix;

    void drawImpl(const ObjectWithMaterial &object) const;
    void drawImpl(const std::vector<ObjectWithMaterial> &object) const;

    void drawWithoutMaterialImpl(const ObjectWithMaterial &object, GLBase::Shader *shader) const;
    void drawWithoutMaterialImpl(const std::vector<ObjectWithMaterial> &object, GLBase::Shader *shader) const;
};

} // namespace GLGeometry

#endif
