#version 330 core

// Output to the G-buffer textures
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

struct Material
{
    vec3 albedo;
    float spec;
};

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform Material material;

void main()
{
    // Position of the fragment in world space
    gPosition = fs_in.FragPos;
    // Normal of the fragment
    gNormal = normalize(fs_in.Normal);
    // Color of the fragment
    gAlbedoSpec.rgb = material.albedo;
    // Specular intensity of the fragment
    gAlbedoSpec.a = material.spec;
}
