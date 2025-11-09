#version 410 core

#define M_PI 3.1415926535897932384626433832795

out vec4 FragColor;

in vec3 TexCoords;

void main()
{
    // Flat color on the sky
    FragColor = vec4(0.43, 0.81, 0.92, 1.);

    // gl_FragDepth = 1;
}
