#ifndef GRAPHICS_WORLD_MANAGER_H
#define GRAPHICS_WORLD_MANAGER_H

#include <memory>
#include <vector>
#include "GLBase.h"
#include "GLTerrainPatch.h"

namespace GLGeometry {

class GraphicsWorldManager {
public:
    template <typename T, typename... Args> GLBase::Camera *setupCamera(Args &&...args) {
        static_assert(std::is_base_of<GLBase::Camera, T>::value, "T must derive from GLBase::Camera");

        mCamera = std::make_unique<T>(std::forward<Args>(args)...);

        return mCamera.get();
    }

    GLBase::Camera *getCamera() { return mCamera.get(); }

    void registerObjectAndMaterial(GLGeometry::GLElemObject *object, GLBase::Material *material);

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

    std::vector<GLObjectWithMaterial> &getListOfObjects() { return mGraphicsObjects; }

    std::vector<std::unique_ptr<Light>> &getListOfLights() { return mLights; }

    GLCubemap *getSkymap() { return mSkymap.get(); }

    void draw();

private:
    std::unique_ptr<GLBase::Camera> mCamera;

    std::vector<GLObjectWithMaterial> mGraphicsObjects;

    std::vector<std::unique_ptr<Light>> mLights;

    std::unique_ptr<GLCubemap> mSkymap;

    GLObjectWithMaterial mTerrain;
};

} // namespace GLGeometry

#endif
