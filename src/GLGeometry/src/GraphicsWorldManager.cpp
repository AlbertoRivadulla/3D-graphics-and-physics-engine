#include "GraphicsWorldManager.h"
#include "GLCubemap.h"

namespace GLGeometry {

GLGeometry::GLElemObject *GraphicsWorldManager::addGeometryObject(std::unique_ptr<GLGeometry::GLElemObject> geometry) {
    mElemenObjects.push_back(std::move(geometry));

    return mElemenObjects.back().get();
}

GLBase::Material *GraphicsWorldManager::addMaterial(std::unique_ptr<GLBase::Material> material) {
    mMaterials.push_back(std::move(material));

    return mMaterials.back().get();
}

void GraphicsWorldManager::registerGraphicsObject(GraphicsObject *graphicsObject) {
    mGraphicsObjects.push_back(graphicsObject);
}

void GraphicsWorldManager::registerTerrainAndMaterial(GLGeometry::GLTerrainPatch *terrainPatch,
                                                      GLBase::Material *material) {
    mTerrain.setObject(terrainPatch, material);
}

Light *GraphicsWorldManager::addLight(std::unique_ptr<Light> light) {
    Light *raw = light.get();
    mLights.push_back(std::move(light));

    return raw;
}

void GraphicsWorldManager::addSkymap(std::unique_ptr<GLCubemap> skymap) { mSkymap = std::move(skymap); }

void GraphicsWorldManager::draw() const {
    // TODO: Delegate this in a terrain drawer class, that will handle drawing different terrain patches depending on
    // the camera frustum

    if (mTerrain.hasGeometry()) {
        mTerrain.draw();
    }

    for (auto object : mGraphicsObjects) {
        object->draw();
    }
}

} // namespace GLGeometry
