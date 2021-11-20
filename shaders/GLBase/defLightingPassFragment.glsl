#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

// Uniforms with the information from the Geometry pass
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

// =======================
uniform vec3 lightPos;
// =======================

// Add lists of lights, and number of them

uniform vec3 viewPos;

void main()
{
    // Get the data from the g-buffer textures
    vec3 FragPos   = texture(gPosition, TexCoords).rgb;
    vec3 Normal    = normalize( texture(gNormal, TexCoords).rgb );
    vec3 Albedo    = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    // Attenuation of the light
    float attenuation = 5. / length(FragPos - lightPos);

    // Ambient lighting
    vec3 ambient = 0.5 * Albedo;

    // Diffuse lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 diffuse = max(dot(lightDir, Normal), 0.) * Albedo;

    // Specular lighting, with Blinn-Phong shading
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // Intensity of the specular component
    float specIntensity = Specular * pow(max(dot(Normal, halfwayDir), 0.), 32.); 
    // Compute the specular component, assuming a white light with a lower indensity
    vec3 specular = vec3(0.3) * specIntensity;

    // Sum all the contributions, with gamma correction
    FragColor = vec4(ambient + attenuation * (diffuse + specular), 1.);
}
