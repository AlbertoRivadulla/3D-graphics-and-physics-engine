#version 410 core

in vec2 TexCoords;
out vec4 color;

// Texture for the character to draw
uniform sampler2D glyphTexture;
// Color for the character
uniform vec3 textColor;

void main()
{
    // The texture only determines the alpha channel
    // I add a step function to make sharp edges
    vec4 sampled = vec4( 1., 1., 1., step( 0.5, texture( glyphTexture, TexCoords ).r ) );

    color = vec4( textColor, 1. ) * sampled;
} 
