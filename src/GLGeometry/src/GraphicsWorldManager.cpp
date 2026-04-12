#include "GraphicsWorldManager.h"

namespace GLGeometry {

void GraphicsWorldManager::registerObjectAndMaterial(GLGeometry::GLElemObject *object, GLBase::Material *material) {
    mGraphicsObjects.push_back({object, material});
}

Light *GraphicsWorldManager::addLight(std::unique_ptr<Light> light) {
    Light *raw = light.get();
    mLights.push_back(std::move(light));

    return raw;
}

void GraphicsWorldManager::draw() {
    // // TODO: Draw the terrain first, when this class has a pointer to the graphics part of the terrain
    // mTerrain->draw();

    for (auto object : mGraphicsObjects) {
        object.material->configShader(object.object->getModelMatrix());

        object.object->draw();
    }
}

} // namespace GLGeometry
