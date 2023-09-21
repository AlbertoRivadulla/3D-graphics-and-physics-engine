#version 410 core
layout(quads, fractional_odd_spacing, ccw) in;

uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec2 TextureCoord[];

out float Height;
out vec3 Position;
out vec3 Normal;

void main()
{
    // Coordinates within the path (from 0 to 1)
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // Texture coordinates of the control points
    vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

    // Interpolate bilinearly the texture coordinates across the patch
    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;

    // Height at the point in the patch, from the heightmap texture
    Height = texture(heightMap, texCoord).y * 64.0 - 16.0;

    // Position coordinates of the control points
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // Compute the normal to the patch surface
    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    // Bilinearly interpolate position coordinates along the patch
    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0 + normal * Height;

    // vs_out.FragPos = vec3(model * vec4(aPos, 1.));
    // vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    // vs_out.TexCoords = aTexCoords;
    // gl_Position = projection * view * vec4(vs_out.FragPos, 1.);

    Position = vec3(model * vec4( p.xyz, 1. ));
    gl_Position = projection * view * vec4( Position, 1. );
    Normal = normal.xyz;
}

