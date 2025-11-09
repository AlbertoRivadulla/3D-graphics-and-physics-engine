#version 410 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 viewInv;
uniform mat4 projectionInv;

void main()
{
    // The quad fills the screen
    gl_Position = vec4(aPos.x, aPos.y, 0., 1.);

    // Compute the texture coordinates inverting the screen coordinates back to 
    // world space
    TexCoords = mat3(viewInv) * (projectionInv * gl_Position).xyz;
}
