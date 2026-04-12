#ifndef GRAPHICS_WORLD_MANAGER_H
#define GRAPHICS_WORLD_MANAGER_H

#include <vector>
#include "GLBase.h"

namespace GLGeometry {

class GraphicsWorldManager {
public:
    void registerObjectAndMaterial(GLGeometry::GLElemObject *object, GLBase::Material *material);

    template <typename T, typename... Args> Light *addLight(Args &&...args) {
        static_assert(std::is_base_of<Light, T>::value, "T must derive from GLBase::Light");

        mLights.push_back(std::make_unique<T>(std::forward<Args>(args)...));

        return mLights.back().get();
    }
    Light *addLight(std::unique_ptr<Light> light);

    std::vector<GLObjectWithMaterial> &getListOfObjects() { return mGraphicsObjects; };

    std::vector<std::unique_ptr<Light>> &getListOfLights() { return mLights; };

    void draw();

private:
    std::vector<GLObjectWithMaterial> mGraphicsObjects;

    std::vector<std::unique_ptr<Light>> mLights;
};

} // namespace GLGeometry

#endif
