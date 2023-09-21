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
};

// in VS_OUT {
//     vec3 FragPos;
//     vec3 Normal;
//     vec2 TexCoords;
// } fs_in;

in float Height;
in vec3 Position;
in vec3 Normal;

uniform Material material;

void main()
{
    // Position of the fragment in world space
    // gPosition = fs_in.FragPos;
    // Output also the z value in the alpha channel of the texture
    gPosition = vec4(Position, gl_FragCoord.z / gl_FragCoord.w);
    // Normal of the fragment
    gNormalEmiss.rgb = normalize(Normal);
    // gNormalEmiss.rgb = vec3(0., 1., 0.);
    // Emissive values
    // gNormalEmiss.a = material.emissive;
    gNormalEmiss.a = 0.;
    // Color of the fragment
    gAlbedoSpec.rgb = material.albedo;
    // gAlbedoSpec.rgb = Normal;
    // Specular intensity of the fragment
    gAlbedoSpec.a = material.spec;
}



// #version 410 core
// 
// in float Height;
// 
// out vec4 FragColor;
// 
// void main()
// {
//     float h = (Height + 16)/64.0f;
//     FragColor = vec4(h, h, h, 1.0);
// }
