#ifndef UTILS_GEOMETRY_H
#define UTILS_GEOMETRY_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "logger.h"

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

inline glm::mat3 computeRotationMatrix(float angle, const glm::vec3 &axis) {
    return glm::mat3(glm::rotate(glm::mat4(1.f), angle, axis));
}

inline glm::mat3 rotateMatrix3(const glm::mat3 &matrix, float angle, const glm::vec3 &axis) {
    glm::mat3 R = glm::mat3(glm::rotate(glm::mat4(1.f), angle, axis));
    return R * matrix * glm::transpose(R);
}

inline glm::mat3 rotateMatrix3(const glm::mat3 &matrix, const glm::quat &rotation) {
    glm::mat3 R = glm::toMat3(rotation);

    return R * matrix * glm::transpose(R);
}

inline glm::mat3 rotateMatrix3(const glm::mat3 &matrix, const glm::mat3 &rotation) {
    return rotation * matrix * glm::transpose(rotation);
}

inline glm::mat4 rotateAroundPoint(const glm::mat4 &matrix, float angle, const glm::vec3 &axis, const glm::vec3 &point) {

    glm::mat4 pivotRotation =
        glm::translate(glm::mat4(1.0f), point) *
        glm::rotate(glm::mat4(1.0f), angle, axis) *
        glm::translate(glm::mat4(1.0f), -point);

    glm::mat4 model = pivotRotation * matrix;
    return model;



    // glm::mat4 m = glm::translate(matrix, -point);
    // m = glm::rotate(m, angle, axis);
    // m = glm::translate(m, point);
    // return m;
}

} // namespace Utils

#endif
