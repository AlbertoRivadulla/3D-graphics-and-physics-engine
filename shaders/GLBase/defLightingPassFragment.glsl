#version 420 core

out vec4 FragColor;

in vec2 TexCoords;

// Uniforms with the information from the Geometry pass
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct DirectionalLight
{
    vec3 color;
    vec3 position;
    vec3 direction;

    float intensity;
    float kLinear;
    float kQuadratic;

    mat4 lightSpaceMatrix;
    sampler2D shadowMap;
};

struct SpotLight
{
    vec3 color;
    vec3 position;
    vec3 direction;

    float intensity;
    float kLinear;
    float kQuadratic;

    float cosAngleInner;
    float cosAngleOuter;
    float radiusMax;

    // mat4 lightSpaceMatrix;
    // sampler2D shadowMap;
};

struct PointLight
{
    vec3 color;
    vec3 position;

    float intensity;
    float kLinear;
    float kQuadratic;

    float radiusMax;

    // mat4 lightSpaceMatrix;
    // sampler2D shadowMap;
};

// Maximum number of lights of each type
const int NR_MAX_DIR_LIGHTS = 4;
const int NR_MAX_SPOT_LIGHTS = 32;
const int NR_MAX_POINT_LIGHTS = 32;

// Lists of lights of each type
uniform DirectionalLight dirLights[NR_MAX_DIR_LIGHTS];
uniform SpotLight spotLights[NR_MAX_SPOT_LIGHTS];
uniform PointLight pointLights[NR_MAX_POINT_LIGHTS];

// Number of lights of each type
uniform int nrDirLights;
uniform int nrSpotLights;
uniform int nrPointLights;

// View position
uniform vec3 viewPos;

// Color of the ambient light, with a default value
uniform vec3 ambientLightColor = vec3(0.1, 0.1, 0.1);

// Functions to compute the shadows
float shadowComputationDirLight(vec3 fragPos, vec3 normal, DirectionalLight light)
{
    // Position of the fragment in light space
    vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(fragPos, 1.);
    // Perform perspective divide
    vec3 projCoordsLightSpace = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range, to use these as the coordinates of the shadow map texture
    projCoordsLightSpace = projCoordsLightSpace * 0.5 + 0.5;
    // Get the closest depth value from the light's perspective at these coodinates
    // in the light's view space
    float closestDepth = texture(light.shadowMap, projCoordsLightSpace.xy).r; 
    // Get the depth of the current fragment from the light's perspective
    float currentDepth = projCoordsLightSpace.z;

    // Compute a bias with the normal vector
    // float shadowBias = max(0.005 * (1.0 - dot(normal, light.direction)), 0.0005);
    // float shadowBias = max(0.00005 * (1.0 - dot(normal, light.direction)), 0.000005);
    float shadowBias = 0.00005;
    // Check whether the current fragment is in the shadow
    float shadow = currentDepth - shadowBias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    // Get the data from the g-buffer textures
    vec3 FragPos   = texture(gPosition, TexCoords).rgb;
    vec3 Normal    = normalize( texture(gNormal, TexCoords).rgb );
    vec3 Albedo    = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    // Ambient lighting
    vec3 lighting = ambientLightColor * Albedo;

    // Compute the lighting due to the directional lights
    for (int i = 0; i < nrDirLights; ++i)
    {
        // Attenuation of the light
        vec3 fragToLight = dirLights[i].position - FragPos;
        float attenuation = dirLights[i].intensity / (1. 
                            + dirLights[i].kLinear * length(fragToLight)
                            + dirLights[i].kQuadratic * dot(fragToLight, fragToLight));

        // Diffuse contribution
        vec3 lightDirection = -1. * normalize(dirLights[i].direction);
        vec3 diffuse = max(dot(lightDirection, Normal), 0.) * Albedo;

        // Specular contribution, with the Blinn-Phong model
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(-dirLights[i].direction + viewDir);
        // Intensity of the specular component
        float specIntensity = Specular * pow(max(dot(Normal, halfwayDir), 0.), 32.); 
        // Color of the specular component
        vec3 specular = specIntensity * Albedo;

        // Compute the shadow
        float shadow = shadowComputationDirLight(FragPos, Normal, dirLights[i]);

        // Sum the two contributions to the total light
        lighting += attenuation * (1. - shadow) * ( diffuse + specular ) * dirLights[i].color;
    }

    // Compute the lighting due to the spot lights
    for (int i = 0; i < nrSpotLights; ++i)
    {
        // Position of the fragment with respect to the light
        vec3 fragToLight = spotLights[i].position - FragPos;
        // Direction of the light, as in a point light
        vec3 lightDirection = normalize(fragToLight);
        // Angle between the center of the light direction and the vector from the
        // light to the fragment
        float cosTheta = -1. * dot(spotLights[i].direction, lightDirection);

        // Attenuation of the light
        float attenuation = spotLights[i].intensity / (1. 
                            + spotLights[i].kLinear * length(fragToLight)
                            + spotLights[i].kQuadratic * dot(fragToLight, fragToLight));
        // The attenuation must be zero if the angle theta is outside the bound
        // of the light source
        // Do a smooth transition from one to zero, in the region between angleInner and angleOuter
        attenuation *= clamp( (cosTheta - spotLights[i].cosAngleOuter) / 
                              (spotLights[i].cosAngleInner - spotLights[i].cosAngleOuter),
                              0., 1.);

        // Diffuse contribution
        vec3 diffuse = max(dot(lightDirection, Normal), 0.) * Albedo;

        // Specular contribution, with the Blinn-Phong model
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDirection + viewDir);
        // Intensity of the specular component
        float specIntensity = Specular * pow(max(dot(Normal, halfwayDir), 0.), 32.); 
        // Color of the specular component
        vec3 specular = specIntensity * Albedo;

        // Sum the two contributions to the total light, multiplied by the 
        // color of the light
        lighting += attenuation * ( diffuse + specular ) * spotLights[i].color;
    }

    // Compute the lighting due to the point lights
    for (int i = 0; i < nrPointLights; ++i)
    {
        // Attenuation of the light
        vec3 fragToLight = pointLights[i].position - FragPos;
        float attenuation = pointLights[i].intensity / (1. 
                            + pointLights[i].kLinear * length(fragToLight)
                            + pointLights[i].kQuadratic * dot(fragToLight, fragToLight));

        // Diffuse contribution
        vec3 lightDirection = normalize(fragToLight);
        vec3 diffuse = max(dot(lightDirection, Normal), 0.) * Albedo;

        // Specular contribution, with the Blinn-Phong model
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDirection + viewDir);
        // Intensity of the specular component
        float specIntensity = Specular * pow(max(dot(Normal, halfwayDir), 0.), 32.); 
        // Color of the specular component
        vec3 specular = specIntensity * Albedo;

        // Sum the two contributions to the total light, multiplied by the 
        // color of the light
        lighting += attenuation * ( diffuse + specular ) * pointLights[i].color;
    }

    // Return the sum of all contributions
    FragColor = vec4(lighting, 1.);


    //
    // float aux = texture(dirLights[0].shadowMap, TexCoords).r;
    // FragColor = vec4(vec3(aux), 1.);
    
}

