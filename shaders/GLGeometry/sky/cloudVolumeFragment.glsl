#version 410 core

#define M_PI 3.1415926535897932384626433832795

out vec4 FragColor;

in vec3 TexCoords;

void main()
{
    // TODO:
    //  - Get angles from the fragment, as in the skybox shader
    //  - Get points of intersection with the cloud volume
    //      - If the starting point of the intersection is further than the depth in the G-buffer, write nothing to this pixel (alpha = 0 or discard; as in pointFragment.glsl)
    //      - Clip the ending distance of the intersection to the minimum between the one computed and the depth in the G-buffer
    //  - Sample the noise texture along the intersection
    //      - ...

    // Flat color on the sky
    FragColor = vec4(0.43, 0.81, 0.92, 0.5);

    gl_FragDepth = 0.;
}
