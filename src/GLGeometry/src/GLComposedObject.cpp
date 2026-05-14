#include "GLComposedObject.h"
#include "src/logger.h"

namespace GLGeometry {

GLComposedObject::GLComposedObject() {}

void GLComposedObject::draw(GLBase::Shader *shader) {
    for (auto &component : mComponents) {
        // Compose the transformation of the GLComposedObject and that of each component
        shader->setMat4("model", mModelMatrix * component.transformMatrix);

        component.object->draw();
    }
}

} // namespace GLGeometry
