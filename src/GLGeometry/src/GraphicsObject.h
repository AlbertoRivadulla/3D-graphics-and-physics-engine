#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include <variant>
#include <vector>
#include "shader.h"
#include "GLElemObject.h"
#include "src/geometry.h"
#include "src/logger.h"

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

    void configureShader(const glm::mat4 &modelMatrix) const { material->configShaderWithModelMatrix(modelMatrix); }
};

class GraphicsObject {
public:
    GraphicsObject();

    bool hasGeometry() const;

    ObjectWithMaterial *setObject(GLGeometry::GLElemObject *object, GLBase::Material *material = nullptr);

    ObjectWithMaterial *addObject(GLGeometry::GLElemObject *object, GLBase::Material *material = nullptr);

    void setGeometry(GLGeometry::GLElemObject *geometry);
    void setGeometry(size_t index, GLGeometry::GLElemObject *geometry);

    void setGlobalModelMatrix(const glm::mat4 &transformMatrix);
    void setGlobalModelMatrix(const glm::vec3 &translation, const float &rotationAngle, const glm::vec3 &rotationAxis,
                              const glm::vec3 &scale);
    void setGlobalModelMatrix(const glm::vec3 &translation, const glm::mat4 &rotationMatrix, const glm::vec3 &scale);

    void setMaterial(GLBase::Material *material);
    void setMaterial(size_t index, GLBase::Material *material);

    void setModelMatrix(glm::mat4 &modelMatrix) { mGlobalModelMatrix = modelMatrix; }

    void setModelMatrix(const glm::mat4 &modelMatrix) { mGlobalModelMatrix = modelMatrix; }

    void setModelMatrix(size_t index, const glm::mat4 &modelMatrix) {
        assert(std::holds_alternative<std::vector<ObjectWithMaterial>>(mData));

        std::get<std::vector<ObjectWithMaterial>>(mData)[index].objectWithTransform.setModelMatrix(modelMatrix);
    }

    template <typename... Args> void setModelMatrix(size_t index, Args &&...args) {
        assert(std::holds_alternative<std::vector<ObjectWithMaterial>>(mData));

        std::get<std::vector<ObjectWithMaterial>>(mData)[index].objectWithTransform.setModelMatrix(
            std::forward<Args>(args)...);
    }

    template <typename... Args> void setModelMatrix(Args &&...args) {
        assert(std::holds_alternative<ObjectWithMaterial>(mData));

        mGlobalModelMatrix = Utils::computeModelMatrix(std::forward<Args>(args)...);
    }

    void draw() const;

    void drawWithoutMaterial(GLBase::Shader *shader) const;

private:
    std::variant<ObjectWithMaterial, std::vector<ObjectWithMaterial>> mData;

    glm::mat4 mGlobalModelMatrix;

    void drawImpl(const ObjectWithMaterial &object) const;
    void drawImpl(const std::vector<ObjectWithMaterial> &objects) const;

    void drawWithoutMaterialImpl(const ObjectWithMaterial &object, GLBase::Shader *shader) const;
    void drawWithoutMaterialImpl(const std::vector<ObjectWithMaterial> &objects, GLBase::Shader *shader) const;
};

} // namespace GLGeometry

#endif
