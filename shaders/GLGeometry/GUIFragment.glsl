#version 410 core

in vec2 TexCoords;
out vec4 color;

// Texture of the GUI to draw
uniform sampler2D GUITexture;
/* // Color for the character */
/* uniform vec3 textColor; */

void main()
{
    color = texture(GUITexture, TexCoords);
} 
