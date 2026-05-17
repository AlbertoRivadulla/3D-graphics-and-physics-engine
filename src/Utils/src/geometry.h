#ifndef UTILS_GEOMETRY_H
#define UTILS_GEOMETRY_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Utils {

inline float wrapAngle(float angle) {
    // Wraps angle to [-π, π]
    while (angle > glm::pi<float>())
        angle -= glm::two_pi<float>();
    while (angle < -glm::pi<float>())
        angle += glm::two_pi<float>();
    return angle;
}

inline glm::vec3 sphericalToCartesian(float yaw, float pitch, float distance) {
    return glm::vec3(cosf(pitch) * cosf(yaw), sinf(pitch), cosf(pitch) * sinf(yaw)) * distance;
}

inline glm::mat4 computeModelMatrix(const glm::vec3 &translation, const float &rotationAngle,
                                    const glm::vec3 &rotationAxis, const glm::vec3 &scale) {
    auto modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::translate(modelMatrix, translation);
    if (rotationAngle != 0.)
        modelMatrix = glm::rotate(modelMatrix, rotationAngle, glm::normalize(rotationAxis));
    modelMatrix = glm::scale(modelMatrix, scale);

    return modelMatrix;
}

inline glm::mat4 computeModelMatrix(const glm::vec3 &translation, const glm::mat4 &rotationMatrix,
                                    const glm::vec3 &scale) {
    auto modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::translate(modelMatrix, translation);
    modelMatrix = modelMatrix * rotationMatrix;
    modelMatrix = glm::scale(modelMatrix, scale);

    return modelMatrix;
}

} // namespace Utils

#endif
