#include "GraphicsWorldManager.h"

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

void GraphicsWorldManager::registerRawObject(GLGeometry::GLElemObject *elemObject) {
    mRawObjects.push_back(elemObject);
}

void GraphicsWorldManager::registerTerrain(GraphicsObject *terrain) {
    mTerrain = terrain;
}

GLBase::Light *GraphicsWorldManager::addLight(std::unique_ptr<GLBase::Light> light) {
    GLBase::Light *raw = light.get();
    mLights.push_back(std::move(light));

    return raw;
}

void GraphicsWorldManager::addSkymap(std::unique_ptr<GLCubemap> skymap) { mSkymap = std::move(skymap); }

void GraphicsWorldManager::draw() const {
    // TODO: Delegate this in a terrain drawer class, that will handle drawing different terrain patches depending on
    // the camera frustum

    if (mTerrain->hasGeometry()) {
        mTerrain->draw();
    }

    // This draws in particular the particle systems
    for (auto *object : mRawObjects) {
        object->draw();
    }

    for (auto *object : mGraphicsObjects) {
        object->draw();
    }
}

} // namespace GLGeometry
