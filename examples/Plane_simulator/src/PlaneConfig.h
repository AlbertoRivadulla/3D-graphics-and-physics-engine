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

enum class PlaneComponentIndex {
    Fuselage = 0,
    PortAileronWing = 1,
    StarboardAileronWing = 2,
    PortFlapWing = 3,
    StarboardFlapWing = 4,
    PortTailElevator = 5,
    StarboardTailElevator = 6,
    TailRudder = 7,
};

constexpr glm::vec3 PlaneCenterOffset = {0., 0., 7.5};
constexpr float AileronSize = 0.75f;
constexpr float FlapSize = 0.75f;

constexpr std::array<PlaneElementConfig, 8> PlaneElementConfigs = {
    PlaneElementConfig(                                                             // Fuselage
        glm::vec3(0.0, 1., 7.5), 0., glm::vec3(0., 0., 1.), glm::vec3(2., 2., 15.), // Position, rotation, scale
        31.8, Physics::InertiaTensors::fromDiagonal({861., 861., 66.}),             // Mass, inertia
        glm::vec3(0., 0., 0.),                                                      // Mass, inertia, CoM
        AeroSurface()                                                               // Aerodynamics
        ),
    PlaneElementConfig( // Port aileron wing
        glm::vec3(12., 1., 10. + AileronSize / 2.), 0., glm::vec3(0., 0., 1.),
        glm::vec3(6., 0.2, 3. - AileronSize),                           // Position, rotation, scale
        6.5, Physics::InertiaTensors::fromDiagonal({10.5, 24.0, 13.9}), // Mass, inertia
        glm::vec3(0., 0., 0. - AileronSize / 2.),                       // CoM offset
        AeroSurface()                                                   // Aerodynamics
        ),
    PlaneElementConfig( // Starboard aileron wing
        glm::vec3(-12., 1., 10. + AileronSize / 2.), 0., glm::vec3(0., 0., 1.),
        glm::vec3(6., 0.2, 3. - AileronSize),                           // Position, rotation, scale
        6.5, Physics::InertiaTensors::fromDiagonal({10.5, 24.0, 13.9}), // Mass, inertia
        glm::vec3(0., 0., 0. - AileronSize / 2.),                       // CoM offset
        AeroSurface()                                                   // Aerodynamics
        ),
    PlaneElementConfig( // Port flap wing
        glm::vec3(5., 1., 10. + FlapSize / 2.), 0., glm::vec3(0., 0., 1.),
        glm::vec3(8, 0.2, 3. - FlapSize),                                // Position, rotation, scale
        7.31, Physics::InertiaTensors::fromDiagonal({12.2, 33.6, 21.9}), // Mass, inertia
        glm::vec3(0., 0., 0. - FlapSize / 2.),                           // CoM offset
        AeroSurface()                                                    // Aerodynamics
        ),
    PlaneElementConfig( // Starboard flap wing
        glm::vec3(-5., 1., 10. + FlapSize / 2.), 0., glm::vec3(0., 0., 1.),
        glm::vec3(8., 0.2, 3. - FlapSize),                               // Position, rotation, scale
        7.31, Physics::InertiaTensors::fromDiagonal({12.2, 33.6, 21.9}), // Mass, inertia
        glm::vec3(0., 0., 0. - FlapSize / 2.),                           // CoM offset
        AeroSurface()                                                    // Aerodynamics
        ),
    PlaneElementConfig( // Port tail elevator
        glm::vec3(2.5, 1., 0.8 + AileronSize / 2.), 0., glm::vec3(0., 0., 1.),
        glm::vec3(3., 0.2, 1.5 - AileronSize),                             // Position, rotation, scale
        2.6, Physics::InertiaTensors::fromDiagonal({0.385, 1.206, 0.837}), // Mass, inertia
        glm::vec3(0., 0., 0. - AileronSize / 2.),                          // CoM offset
        AeroSurface()                                                      // Aerodynamics
        ),
    PlaneElementConfig( // Starboard tail elevator
        glm::vec3(-2.5, 1., 0.8 + AileronSize / 2.), 0., glm::vec3(0., 0., 1.),
        glm::vec3(3., 0.2, 1.5 - AileronSize),                             // Position, rotation, scale
        2.6, Physics::InertiaTensors::fromDiagonal({0.385, 1.206, 0.837}), // Mass, inertia
        glm::vec3(0., 0., 0. - AileronSize / 2.),                          // CoM offset
        AeroSurface()                                                      // Aerodynamics
        ),
    PlaneElementConfig( // Rudder
        glm::vec3(0., 3.5, 0.8), glm::radians(90.), glm::vec3(0., 0., 1.),
        glm::vec3(3., 0.2, 1.5),                                            // Position, rotation, scale
        2.93, Physics::InertiaTensors::fromDiagonal({1.942, 0.718, 1.262}), // Mass, inertia
        glm::vec3(0., 0., 0.),                                              // CoM offset
        AeroSurface()                                                       // Aerodynamics
        ),
};

constexpr auto PlaneElemsWithAileronIndices =
    std::array{PlaneComponentIndex::PortAileronWing, PlaneComponentIndex::StarboardAileronWing,
               PlaneComponentIndex::PortTailElevator, PlaneComponentIndex::StarboardTailElevator};

constexpr auto PlaneElemsWithFlapIndices =
    std::array{PlaneComponentIndex::PortFlapWing, PlaneComponentIndex::StarboardFlapWing};

#endif
