#version 410 core

#define M_PI 3.1415926535897932384626433832795

out vec4 FragColor;

in vec3 TexCoords;

void main()
{
    // Flat color on the sky
    // FragColor = vec4(0.43, 0.81, 0.92, 1.);

    float x = TexCoords.x;
    float y = TexCoords.y;
    float z = TexCoords.z;
    float phi = (atan(y , x) + M_PI) / (2 * M_PI);
    float r2 = sqrt(x*x + y*y);
    float theta = (atan(z, r2) + M_PI / 2) / (M_PI);
    FragColor = vec4(phi, 0., theta, 1.);

    // gl_FragDepth = 1;
}
