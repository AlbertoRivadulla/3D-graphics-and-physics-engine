#ifndef INERTIA_TENSORS_H
#define INERTIA_TENSORS_H

#include <glm/glm.hpp>

namespace Physics {
namespace InertiaTensors {

inline constexpr glm::mat3 fromDiagonal(glm::vec3 diagonal) {
    return glm::mat3(diagonal.x, 0., 0., 0., diagonal.y, 0., 0., 0., diagonal.z);
}

inline constexpr glm::mat3 sphere(float mass, float radius) {
    float I = (2. / 5.) * mass * radius * radius;

    return fromDiagonal(glm::vec3(I, I, I));
}

inline constexpr glm::mat3 box(float mass, float lx, float ly, float lz) {
    float Ix = (1. / 12.) * mass * (ly * ly + lz * lz);
    float Iy = (1. / 12.) * mass * (lx * lx + lz * lz);
    float Iz = (1. / 12.) * mass * (lx * lx + ly * ly);

    return fromDiagonal(glm::vec3(Ix, Iy, Iz));
}

inline constexpr glm::mat3 cylinder(float mass, float r, float h) {
    float Ir = (1. / 12.) * mass * (3 * r * r + h * h);
    float Iz = 0.5 * mass * r * r;

    return fromDiagonal(glm::vec3(Ir, Ir, Iz));
}

} // namespace InertiaTensors
} // namespace Physics

#endif
