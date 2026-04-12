#include "GraphicsWorldManager.h"
#include "GLCubemap.h"

namespace GLGeometry {

void GraphicsWorldManager::registerObjectAndMaterial(GLGeometry::GLElemObject *object, GLBase::Material *material) {
    mGraphicsObjects.push_back({object, material});
}

void GraphicsWorldManager::registerTerrainAndMaterial(GLGeometry::GLTerrainPatch *terrainPatch,
                                                      GLBase::Material *material) {
    mTerrain.object = terrainPatch;
    mTerrain.material = material;
}

Light *GraphicsWorldManager::addLight(std::unique_ptr<Light> light) {
    Light *raw = light.get();
    mLights.push_back(std::move(light));

    return raw;
}

void GraphicsWorldManager::addSkymap(std::unique_ptr<GLCubemap> skymap) { mSkymap = std::move(skymap); }

void GraphicsWorldManager::draw() {
    // TODO: Delegate this in a terrain drawer class, that will handle drawing different terrain patches depending on
    // the camera frustum
    if (mTerrain.object != nullptr) {
        mTerrain.material->configShader(mTerrain.object->getModelMatrix());

        mTerrain.object->draw();
    }

    for (auto object : mGraphicsObjects) {
        object.material->configShader(object.object->getModelMatrix());

        object.object->draw();
    }
}

} // namespace GLGeometry
