#ifndef GRAPHICS_WORLD_MANAGER_H
#define GRAPHICS_WORLD_MANAGER_H

#include <vector>
#include "GLBase.h"

namespace GLGeometry {

class GraphicsWorldManager {
public:
    void registerObjectAndMaterial(GLGeometry::GLElemObject *object,
                                   GLBase::Material *material);

    std::vector<GLObjectWithMaterial>& getListOfObjects() {
        return mGraphicsObjects;
    };

    void draw();

private:
    std::vector<GLObjectWithMaterial> mGraphicsObjects;
};

} // namespace GLGeometry

#endif
