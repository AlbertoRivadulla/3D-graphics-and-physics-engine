#include "GLBase.h"
#include "GLGeometry.h"
#include "PlaneSim.h"
#include "glm/matrix.hpp"
#include "utils.h"

// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

int main()
{
    PlaneSim planeSim(SCR_WIDTH, SCR_HEIGHT, "Plane simulator", 1.f);

    planeSim.run();

    return 0;
}

/*
   - The code to make the updates of the objects with physics slower is located in
     the file PhysicsWorld.cpp
   - 
*/
