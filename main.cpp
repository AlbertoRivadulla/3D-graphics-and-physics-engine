#include "GLBase.h"
#include "GLGeometry.h"
#include "GLSandbox.h"

// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Linear interpolation function
float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

int main()
{
    // Create the sandbox
    GLSandbox sandbox(SCR_WIDTH, SCR_HEIGHT, "Title");

    // Start the application
    sandbox.run();

    return 0;
}
