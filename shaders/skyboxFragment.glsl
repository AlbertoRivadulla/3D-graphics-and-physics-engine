#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, TexCoords);

    /* vec3 color = mix(0.5 * vec3(0.2, 0.9, 0.2), vec3(0., 0.7, 0.9), TexCoords.g + 0.5); */
    /* FragColor = vec4(color, 1.); */
}
