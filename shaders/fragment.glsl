#version 330 core

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

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // Value for the attenuation of the light
    float attenuation = 5. / length(fs_in.FragPos - lightPos);

    // Ambient lighting
    vec3 ambient = 0.5 * material.albedo;

    // Diffuse lighting
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    vec3 diffuse = max(dot(lightDir, normal), 0.) * material.albedo;

    // Specular lighting, with Blinn-Phong shading
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // Intensity of the specular component
    float specIntensity = material.spec *  pow(max(dot(normal, halfwayDir), 0.), 32.); 
    // Compute the specular component, assuming a white light with a lower indensity
    vec3 specular = vec3(0.3) * specIntensity;

    // Sum all the contributions, with gamma correction
    FragColor = vec4(ambient + attenuation * (diffuse + specular), 1.);
}
