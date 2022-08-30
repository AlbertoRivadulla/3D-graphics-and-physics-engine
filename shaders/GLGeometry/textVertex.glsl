#version 410 core

// The input is <vec2 pos, vec2 tex>
layout (location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    // The quad is located at the position of the screen
    gl_Position = projection * vec4( vertex.xy, 0., 1. );

    // The last 2 elements of the input are the texture coordinates
    TexCoords = vertex.zw;
}
