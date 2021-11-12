#version 330 core

out vec4 FragColor;

uniform vec3 color;

void main()
{
    FragColor = vec4(color, 1.);

    // Bring the line to the front
    gl_FragDepth = 0.;
}
