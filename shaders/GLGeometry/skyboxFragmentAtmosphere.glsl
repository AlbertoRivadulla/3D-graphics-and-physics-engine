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

    // float phi = (atan(y , x) + M_PI) / (2 * M_PI);
    // float r2 = sqrt(x*x + z*z);
    // float theta = (atan(z, r2) + M_PI / 2) / (M_PI);
    // FragColor = vec4(phi, 0., theta, 1.);

    // // Second way to compute the angles (phi goes around the vertical axis)
    // float phi = (atan(z , x) + M_PI) / (2 * M_PI);
    // float r2 = sqrt(x*x + z*z);
    // float theta = (atan(y, r2) + M_PI / 2) / (M_PI);
    // FragColor = vec4(phi, 0., theta, 1.);

    // FragColor = vec4(0., y / 2 + 0.5, 0., 1.);
    FragColor = vec4(x / 2 + 0.5, 0., 0., 1.);

    // gl_FragDepth = 1;
}
