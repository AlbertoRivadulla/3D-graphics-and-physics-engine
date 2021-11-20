#version 330 core

// Shader used to draw the texture attachment of a FBO to the screen

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0., 1.0);
    TexCoords = aTexCoords;
}
