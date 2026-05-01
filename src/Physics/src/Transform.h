#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Physics {

struct Transform {
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat orientation;

    glm::mat4 modelMatrix;
};

} // namespace Physics

#endif
