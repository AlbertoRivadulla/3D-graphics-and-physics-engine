#ifndef GRAPHICS_WORLD_MANAGER_H
#define GRAPHICS_WORLD_MANAGER_H

#include <memory>
#include <vector>
#include "GLBase.h"
#include "GLElemObject.h"
#include "GLTerrainPatch.h"
#include "GraphicsObject.h"

namespace GLGeometry {

class GraphicsWorldManager {
public:
    template <typename T, typename... Args> GLGeometry::GLElemObject *addGeometryObject(Args &&...args) {
        static_assert(std::is_base_of<GLGeometry::GLElemObject, T>::value,
                      "T must derive from GLGeometry::GLElemObject");

        mElemenObjects.push_back(std::make_unique<T>(std::forward<Args>(args)...));

        return mElemenObjects.back().get();
    }
    GLGeometry::GLElemObject *addGeometryObject(std::unique_ptr<GLGeometry::GLElemObject>);

    template <typename T, typename... Args> GLBase::Material *addMaterial(Args &&...args) {
        static_assert(std::is_base_of<GLBase::Material, T>::value, "T must derive from GLBase::Material");

        mMaterials.push_back(std::make_unique<T>(std::forward<Args>(args)...));

        return mMaterials.back().get();
    }
    GLBase::Material *addMaterial(std::unique_ptr<GLBase::Material>);

    template <typename T, typename... Args> GLBase::Camera *setupCamera(Args &&...args) {
        static_assert(std::is_base_of<GLBase::Camera, T>::value, "T must derive from GLBase::Camera");

        mCamera = std::make_unique<T>(std::forward<Args>(args)...);

        return mCamera.get();
    }

    GLBase::Camera *getCamera() { return mCamera.get(); }

    void registerGraphicsObject(GraphicsObject *graphicsObject);

    void registerTerrainAndMaterial(GLGeometry::GLTerrainPatch *terrainPatch, GLBase::Material *material);

    template <typename T, typename... Args> Light *addLight(Args &&...args) {
        static_assert(std::is_base_of<Light, T>::value, "T must derive from GLBase::Light");

        mLights.push_back(std::make_unique<T>(std::forward<Args>(args)...));

        return mLights.back().get();
    }
    Light *addLight(std::unique_ptr<Light> light);

    template <typename T, typename... Args> void addSkymap(Args &&...args) {
        static_assert(std::is_base_of<GLCubemap, T>::value, "T must derive from GLGeometry::GLCubemap");

        mSkymap = std::make_unique<T>(std::forward<Args>(args)...);
    }
    void addSkymap(std::unique_ptr<GLCubemap> skymap);

    std::vector<GraphicsObject *> &getListOfObjects() { return mGraphicsObjects; }

    std::vector<std::unique_ptr<Light>> &getListOfLights() { return mLights; }

    GLCubemap *getSkymap() { return mSkymap.get(); }

    void draw() const;

private:
    std::unique_ptr<GLBase::Camera> mCamera;

    std::vector<std::unique_ptr<GLGeometry::GLElemObject>> mElemenObjects;

    std::vector<std::unique_ptr<GLBase::Material>> mMaterials;

    std::vector<GraphicsObject *> mGraphicsObjects;

    std::vector<std::unique_ptr<Light>> mLights;

    std::unique_ptr<GLCubemap> mSkymap;

    GraphicsObject mTerrain;
};

} // namespace GLGeometry

#endif
