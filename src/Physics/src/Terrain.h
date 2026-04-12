#ifndef TERRAIN_H
#define TERRAIN_H

#include "GLBase.h"
#include "GLGeometry.h"
#include "GLTerrainPatch.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics {
class Terrain {
public:
    Terrain();

    // Add a terrain patch from an image
    void addPatchFromTexture(const std::string &heightmapTexPath, float hScale, float vScale, float yShift = 0.f);
    void addPatchPlaneTessellated(float hScale, float vScale, float yShift = 0.f);
    void addPatchFromTextureTessellated(const std::string &heightmapTexPath, float hScale, float vScale,
                                        float yShift = 0.f);
    void addPatchFromHeightDataTessellated(float *heightMapData, int width, int height, float hScale, float vScale,
                                           float yShift = 0.f);

    template <typename T, typename... Args> void addMaterial(Args &&...args) {
        static_assert(std::is_base_of<GLBase::Material, T>::value, "T must derive from GLBase::Material");

        auto material = std::make_unique<T>(std::forward<Args>(args)...);
        addMaterial(std::move(material));
    }
    void addMaterial(std::unique_ptr<GLBase::Material>);

    GLTerrainPatch *getTerrainPatch() { return mTerrainPatch.get(); }
    Material *getMaterial() { return mMaterial.get(); }

    // Get the tessellation shader
    Shader &getTessellationShader();

private:
    // // List of elementary objects in the corresponding sandbox
    // std::vector<GLElemObject*>* mElementaryObjects;

    // Data for the height and normal maps
    std::vector<float> mDataHeight;
    std::vector<float> mDataNormal;

    // Material
    std::unique_ptr<Material> mMaterial;

    // Terrain patch
    std::unique_ptr<GLTerrainPatch> mTerrainPatch;

    // Shader for tessellation
    Shader mTessellationShader;

    // Compute the normal map given an array of data for the height map
    void computeNormalmapData(int width, int height, float hScale, float vScale);
};
} // namespace Physics

#endif
