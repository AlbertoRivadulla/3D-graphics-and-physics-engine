#version 410 core

// Output to the G-buffer textures
// layout (location = 0) out vec3 gPosition;
layout (location = 0) out vec4 gPosition;
// layout (location = 1) out vec3 gNormal;
layout (location = 1) out vec4 gNormalEmiss;
layout (location = 2) out vec4 gAlbedoSpec;

struct Material
{
    vec3 albedo;
    float spec;
    float emissive;

    bool hasTexAlbedo;
    sampler2D texAlbedo;
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
    gNormalEmiss.rgb = normalize(fs_in.Normal);
    // Emissive values
    gNormalEmiss.a = material.emissive;
    // Color of the fragment
    if ( material.hasTexAlbedo )
        gAlbedoSpec.rgb = texture( material.texAlbedo, fs_in.TexCoords ).rgb;
    else
        gAlbedoSpec.rgb = material.albedo;

    // Specular intensity of the fragment
    gAlbedoSpec.a = material.spec;
}
