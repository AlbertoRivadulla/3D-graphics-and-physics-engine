#version 420 core

out vec4 FragColor;

in vec3 TexCoords;

void main()
{
    // Flat color on the sky
    FragColor = vec4(0.43, 0.81, 0.92, 1.);

    // gl_FragDepth = 1;
}
