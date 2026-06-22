#ifndef PLANE_CONFIG_H
#define PLANE_CONFIG_H

#include "AeroSurface.h"
#include "InertiaTensors.h"

#include <glm/glm.hpp>
#include <array>

struct PlaneElementConfig {
    glm::vec3 relativePos;
    float relativeRotationAngle;
    glm::vec3 relativeRotationAxis;
    glm::vec3 scale;

    float mass;
    glm::mat3 localInertiaTensor;
    glm::vec3 centerOfMassWrtRelativePos;

    AeroSurface aeroSurfaceParams;
};

enum class ComponentIndex {
    Fuselage = 0,
    PortAileronWing = 1,
    StarboardAileronWing = 2,
    PortFlapWing = 3,
    StarboardFlapWing = 4,
    PortTailElevator = 5,
    StarboardTailElevator = 6,
    TailRudder = 7,
};

constexpr std::array<PlaneElementConfig, 2> PlaneElementConfigs = {
    PlaneElementConfig(glm::vec3(1., 0., 0.), 0.f, glm::vec3(0., 0., 1.), glm::vec3(1., 2., 1.), 1.,
                       Physics::InertiaTensors::box(1., 2., 1., 1.), glm::vec3(0., 0., 0.), AeroSurface()),
    PlaneElementConfig(glm::vec3(-1., 0., 0.), 45.f, glm::vec3(0., 1., 1.), glm::vec3(1., 1., 2.), 2.,
                       Physics::InertiaTensors::box(2., 1., 1., 2.), glm::vec3(-1., 0., 0.), AeroSurface()),
};

#endif
