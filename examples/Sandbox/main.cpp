#include "GLBase.h"
#include "GLGeometry.h"
#include "Sandbox.h"
#include "glm/matrix.hpp"
#include "utils.h"

// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

int main()
{
    // Create the sandbox
    GLSandbox sandbox(SCR_WIDTH, SCR_HEIGHT, "Sandbox", 1.f);

    // Start the application
    sandbox.run();

    return 0;
}



/*
   - The code to make the updates of the objects with physics slower is located in
     the file PhysicsWorld.cpp
   - 
*/
