#version 330 core

out vec4 FragColor;

uniform vec3 color;

in vec2 LocalCoords;

void main()
{
    float distSquare = LocalCoords.x * LocalCoords.x + LocalCoords.y * LocalCoords.y;
    // Draw only the points that are inside the circle, with the given color
    if (distSquare < 1)
    {
        // Draw a black line at the edge of the circle, with a step function
        FragColor = vec4( mix(color, vec3(0.), step(1. - 0.4, distSquare)), 1. );
    }
    else
        discard;

    // Bring the point to the front
    gl_FragDepth = 0.;
}
