#version 410 core

// Output to the G-buffer textures
// layout (location = 0) out vec3 gPosition;
layout (location = 0) out vec4 gPosition;
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
    // gPosition = fs_in.FragPos;
    // Output also the z value in the alpha channel of the texture
    // gPosition = vec4(fs_in.FragPos, gl_FragCoord.z);
    gPosition = vec4(fs_in.FragPos, gl_FragCoord.z / gl_FragCoord.w);
    // Normal of the fragment
    gNormal = normalize(fs_in.Normal);
    // Color of the fragment
    gAlbedoSpec.rgb = material.albedo;
    // Specular intensity of the fragment
    gAlbedoSpec.a = material.spec;
}
