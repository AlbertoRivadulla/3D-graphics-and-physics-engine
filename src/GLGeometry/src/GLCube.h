#ifndef GLCUBE_H
#define GLCUBE_H

#include "GLElemObject.h"

using namespace GLBase;

namespace GLGeometry {
class GLCube : public GLElemObject {
private:
    // Data of the mesh
    // std::vector<Vertex> mVertices;
    std::vector<unsigned int> mIndices;

public:
    GLCube();

    void draw();
};
} // namespace GLGeometry

#endif
