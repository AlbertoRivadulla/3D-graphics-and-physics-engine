#ifndef GRAPHICS_WORLD_MANAGER_H
#define GRAPHICS_WORLD_MANAGER_H

#include <vector>
#include "GLBase.h"

namespace GLGeometry {

struct GLObjectWithMaterial {
    GLGeometry::GLElemObject *object;
    GLBase::Material *material;
};

class GraphicsWorldManager {
public:
    void registerObjectAndMaterial(GLGeometry::GLElemObject *object,
                                   GLBase::Material *material);

    void draw();

private:
    std::vector<GLObjectWithMaterial> mGraphicsObjects;
};

} // namespace GLGeometry

#endif
