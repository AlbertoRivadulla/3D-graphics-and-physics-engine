#version 410 core
layout (location = 0) in vec3 aPos;

// uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    // Only do the world space transformation here
    gl_Position = model * vec4(aPos, 1.);
}
