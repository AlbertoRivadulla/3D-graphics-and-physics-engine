#version 410 core

layout (location = 0) out vec4 FragColor;
// out vec4 FragColor;

in vec2 TexCoords;

// Uniforms with the information from the Geometry pass
uniform sampler2D gPosition;
uniform sampler2D gNormalEmiss;
uniform sampler2D gAlbedoSpec;

// Maximum cascade levels for the directional light
const int NR_MAX_CASCADE_LEVELS = 8;

struct DirectionalLight
{
    vec3 color;
    vec3 position;
    vec3 direction;

    float intensity;
    float kLinear;
    float kQuadratic;

    mat4 lightSpaceMatrix;
    sampler2DArray shadowMap;
    // int nrCascadeLevels;
    float cascadeDistances[NR_MAX_CASCADE_LEVELS + 1];
};

struct SpotLight
{
    vec3 color;
    vec3 position;
    vec3 direction;
    vec3 upDirection;

    float intensity;
    float kLinear;
    float kQuadratic;

    float cosAngleInner;
    float cosAngleOuter;
    float radiusMax;

    mat4 lightSpaceMatrix;
    /* sampler2D shadowMap; */
};

uniform sampler2D spotShadowMap;

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

// Light space matrices for the cascaded shadow maps
// layout (std140, binding = 0) uniform LightSpaceMatrices
layout (std140) uniform LightSpaceMatrices
{
    // This allows for a maximum of 8 cascades
    mat4 lightSpaceMatrices[16];
};
/* uniform sampler2DArray directionalShadowMap; */

// Number of levels in the cascaded shadow maps
uniform int nrCascadeLevels;

// Number of lights of each type
uniform int nrDirLights = 0;
uniform int nrSpotLights = 0;
uniform int nrPointLights = 0;

// View position
uniform vec3 viewPos;

// Color of the ambient light, with a default value
uniform vec3 ambientLightColor = vec3(0.1, 0.1, 0.1);

// Functions to compute the shadow of a directional light with cascaded shadowmaps
float shadowComputationDirLight(vec3 fragPos, vec3 normal, float depth, DirectionalLight light)
// float shadowComputationDirLight(vec3 fragPos, vec3 normal, float depth, int index)
{
    // Check what level of the cascade map the fragment is in
    int level = -1;
    for (int j = 0; j < nrCascadeLevels; ++j)
    {
        if (depth < light.cascadeDistances[j+1])
        {
            level = j;
            break;
        }
    }

    // Position of the fragment in light space, with the corresponding light
    // space matrix
    /* vec4 fragPosLightSpace = lightSpaceMatrices[level] * vec4(fragPos, 1.); */
    vec4 fragPosLightSpace = lightSpaceMatrices[level] * vec4(fragPos, 1.);

    // Perform perspective divide
    vec3 projCoordsLightSpace = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range, to use these as the coordinates of the shadow map texture
    projCoordsLightSpace = projCoordsLightSpace * 0.5 + 0.5;
    // Get the closest depth value from the light's perspective at these coodinates
    // in the light's view space
    // float closestDepth = texture(light.shadowMap, projCoordsLightSpace.xy).r; 
    // float closestDepth = texture(light.shadowMap, vec3(projCoordsLightSpace.xy, 0.)).r; 
    float closestDepth = texture(light.shadowMap, vec3(projCoordsLightSpace.xy, level)).r; 
    // Get the depth of the current fragment from the light's perspective
    float currentDepth = projCoordsLightSpace.z;

    // Calculate the bias based on the depth map resolution and slope
    float bias = max(0.025 * (1.0 + dot(normal, light.direction)), 0.0025);
    if (level == nrCascadeLevels)
    {
        bias *= 1. / (light.cascadeDistances[nrCascadeLevels] * 0.5f);
    }
    else
    {
        bias *= 1. / (light.cascadeDistances[level + 1] * 0.5f);
    }

    // PCF (percentage closer filtering)
    // This averages over the 9 surrounding pixels of the shadow map, to make 
    // softer shadows
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(light.shadowMap, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(light.shadowMap, vec3(projCoordsLightSpace.xy + vec2(x, y) * texelSize, float(level))).r; 
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}

float shadowComputationSpotLight(vec3 fragPos, vec3 normal, float depth, SpotLight light)
{
    // Position of the fragment in light space, with the corresponding light
    // space matrix
    vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(fragPos, 1.);

    // Perform perspective divide
    vec3 projCoordsLightSpace = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range, to use these as the coordinates of the shadow map texture
    projCoordsLightSpace = projCoordsLightSpace * 0.5 + 0.5;
    // Get the closest depth value from the light's perspective at these coodinates
    // in the light's view space
    float closestDepth = texture(spotShadowMap, projCoordsLightSpace.xy).r;

    // Get the distance from the current fragment to the light
    float currentDepth = length(fragPos - light.position) / light.radiusMax;

    // Calculate the bias based on the depth map resolution and slope
    float bias = max(0.025 * (1.0 + dot(normal, light.direction)), 0.0025);

    // PCF (percentage closer filtering)
    // This averages over the 9 surrounding pixels of the shadow map, to make 
    // softer shadows
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(spotShadowMap, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(spotShadowMap, projCoordsLightSpace.xy + vec2(x, y) * texelSize).r; 
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}

void main()
{
    // Get the data from the g-buffer textures
    vec3 FragPos   = texture(gPosition, TexCoords).rgb;
    float Depth    = texture(gPosition, TexCoords).a;
    vec3 Normal    = normalize( texture(gNormalEmiss, TexCoords).rgb );
    float Emissive = texture(gNormalEmiss, TexCoords).a;
    vec3 Albedo    = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    // If the pixel has an emissive component, the color is simply that of the
    // albedo multiplied by the emissive factor
    if (Emissive > 0.)
    {
        FragColor = vec4(Albedo * Emissive, 1.);
    }
    else
    {
        // Ambient lighting
        vec3 lighting = ambientLightColor * Albedo;

        // Compute the lighting due to the directional lights
        // for (int i = 0; i < nrDirLights; ++i)
        // {
            // Attenuation of the light
            vec3 fragToLight = dirLights[0].position - FragPos;
            float attenuation = dirLights[0].intensity / (1. 
                                + dirLights[0].kLinear * length(fragToLight)
                                + dirLights[0].kQuadratic * dot(fragToLight, fragToLight));

            // Diffuse contribution
            vec3 lightDirection = -1. * dirLights[0].direction;
            vec3 diffuse = max(dot(lightDirection, Normal), 0.) * Albedo;

            // Specular contribution, with the Blinn-Phong model
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 halfwayDir = normalize(-dirLights[0].direction + viewDir);
            // Intensity of the specular component
            float specIntensity = Specular * pow(max(dot(Normal, halfwayDir), 0.), 32.); 
            // Color of the specular component
            vec3 specular = specIntensity * Albedo;

            // Compute the shadow
            float shadow = shadowComputationDirLight(FragPos, Normal, Depth, dirLights[0]);

            // Sum the two contributions to the total light
            lighting += attenuation * (1. - shadow) * ( diffuse + specular ) * dirLights[0].color;
        // }

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

            // Compute the shadow
            float shadow = shadowComputationSpotLight(FragPos, Normal, Depth, spotLights[i]);

            // Sum the two contributions to the total light
            lighting += attenuation * (1. - shadow) * ( diffuse + specular ) * spotLights[i].color;
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
    }
}
