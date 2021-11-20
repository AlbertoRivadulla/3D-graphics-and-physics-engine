#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    // Get the color in the skybox from the texture
    FragColor = texture(skybox, TexCoords);
}
