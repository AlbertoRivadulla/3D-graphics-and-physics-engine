#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>

namespace Physics {

struct Transform {
    glm::vec3 position;
    glm::vec3 scale;
    glm::mat4 rotationMatrix;

    glm::mat4 modelMatrix;
};

} // namespace Physics

#endif
