#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;

void main()
{
    TexCoord = aTex;
    // Convert xyz to xyzw, with w = 1
    gl_Position = vec4(aPos, 1.0);
}
