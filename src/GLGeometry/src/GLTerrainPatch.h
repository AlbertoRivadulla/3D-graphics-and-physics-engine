#ifndef GLTERRAINPATCH_H
#define GLTERRAINPATCH_H

#include "GLGeometry.h"
#include "GLElemObject.h"

namespace Physics {
class Terrain;
}

namespace GLGeometry {
class GLTerrainPatch : public GLElemObject {
    friend class Physics::Terrain;
protected:
    // This class will use an element buffer object
    unsigned int mEBO;

    // Data of the mesh
    // std::vector<Vertex> mVertices;
    std::vector<unsigned int> mIndices;

    // Number of strips and vertices per strip
    unsigned int mNumStrips;
    unsigned int mNumVertsPerStrip;

    // Height map and Normal map texture
    unsigned int mHeightmapTex;
    unsigned int mNormalmapTex;

public:
    GLTerrainPatch();
    GLTerrainPatch(unsigned char *data, int width, int height, int nChannels);

    void draw(GLBase::Shader *shader);
};

class GLTerrainTessellated : public GLTerrainPatch {
private:
    // Number of vertices per patch
    unsigned int mNrVertsPerPatch;
    // Number of patches along each direction
    unsigned int mNrPatches;

public:
    // Constructor
    GLTerrainTessellated(int width, int height);

    // Function to render
    void draw(GLBase::Shader *shader) override;
};

} // namespace GLGeometry

#endif
