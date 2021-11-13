#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

out vec4 FragColor;

uniform vec3 color;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // Value for the attenuation of the light
    float attenuation = 5. / length(fs_in.FragPos - lightPos);

    // Ambient lighting
    vec3 ambient = 0.5 * color;

    // Diffuse lighting
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    vec3 diffuse = max(dot(lightDir, normal), 0.) * color;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    // Float for the intensity of the specular color
    float spec = 1.;
    // Blinn-Phong shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.), 128.); 
    // Compute the specular component, assuming a white light with a lower indensity
    vec3 specular = vec3(0.1) * spec;

    // Sum all the contributions, with gamma correction
    // FragColor = vec4(ambient + diffuse + specular, 1.);
    FragColor = vec4(ambient + attenuation * (diffuse + specular), 1.);
}
