#version 410 core

// Shader used to draw the texture attachment of a FBO to the screen

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, TexCoords);
}
