#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform vec2 winSize;
uniform int radius;

out vec2 LocalCoords;

void main()
{
    vec2 offset = radius / winSize;

    // Generate a quad in screen coordinates
    gl_Position = gl_in[0].gl_Position + gl_in[0].gl_Position.w * radius * vec4(-offset.x, -offset.y, 0., 0.);
    LocalCoords = vec2(-1, -1);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + gl_in[0].gl_Position.w * radius * vec4( offset.x, -offset.y, 0., 0.);
    LocalCoords = vec2( 1, -1);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + gl_in[0].gl_Position.w * radius * vec4(-offset.x,  offset.y, 0., 0.);
    LocalCoords = vec2(-1,  1);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + gl_in[0].gl_Position.w * radius * vec4( offset.x,  offset.y, 0., 0.);
    LocalCoords = vec2( 1,  1);
    EmitVertex();
    EndPrimitive();
}
