#version 410 core

#define M_PI 3.1415926535897932384626433832795

in vec3 TexCoords;
out vec4 FragColor;

uniform vec3 cameraPos;
uniform float sunPhi;
uniform float sunTheta;

const int numInScatteringPoints = 10;
const int numOpticalDepthPoints = 10;
const float densityFalloff = 5.;
const float epsilon = 10.f;

const float earthRadius = 6.378E6;
const float atmosphereHeight = 1.E6;
const float atmosphereRadius = earthRadius + atmosphereHeight;
const float atmosphereRadiusSq = atmosphereRadius * atmosphereRadius;

const vec3 wavelengths = vec3(700, 530, 440);
const float scatteringStrength = 3.;

// The apparent size of the Sun in the Earth's atmosphere is ~0.54 deg = ~0.0094 rad
const float sunAngularSize = 0.0094;
const float bloomIntensity = 0.6;
const vec3 sunColor = vec3(1.0, 0.9, 0.6);
const vec3 sunBloomColor = vec3(1.0, 0.8, 0.4);

float distThroughAtmosphere(vec3 position, vec3 dir)
{
    float pDotN = dot(position, dir);

    return -pDotN + sqrt(pDotN*pDotN - dot(position, position) + atmosphereRadiusSq);
}

float densityAtHeight(float height)
{
    float height0To1 = (height - earthRadius) / atmosphereHeight;
    return exp(-height0To1 * densityFalloff) * (1 - height0To1);
}

float opticalDepth(vec3 inScatterPoint, vec3 direction, float rayLength)
{
    vec3 densitySamplePoint = inScatterPoint;
    float stepSize = rayLength / (numOpticalDepthPoints - 1);
    float opticalDepth = 0.;

    for (int i = 0; i < numOpticalDepthPoints; ++i)
    {
        float localDensity = densityAtHeight(sqrt(dot(densitySamplePoint, densitySamplePoint)));
        opticalDepth += localDensity * stepSize;
        densitySamplePoint += direction * stepSize;
    }

    return opticalDepth;
}

vec3 calculateLight(float cameraHeight, float phi, float theta, vec3 scatterCoeffs)
{
    vec3 inScatteredLight = vec3(0., 0., 0.);

    // Compute the absolute height from the center of the Earth
    cameraHeight += earthRadius;

    vec3 dirToSun = vec3(cos(sunPhi)*cos(sunTheta), sin(sunPhi)*cos(sunTheta), sin(sunTheta));
    vec3 cameraPos = vec3(0., 0., cameraHeight);
    vec3 rayDir = vec3(cos(phi)*cos(theta), sin(phi)*cos(theta), sin(theta));
    vec3 inScatterPoint = cameraPos;

    float rayLength = distThroughAtmosphere(cameraPos, rayDir) - epsilon;
    if (rayLength < 0.)
    {
        return vec3(0., 0., 0.);
    }
    // float rayLength = abs(distThroughAtmosphere(cameraPos, rayDir)) - epsilon;
    float rayStepSize = rayLength / (numInScatteringPoints - 1);

    // Sum the amount of light that reaches the camera from each scattering point
    for (int i = 0; i < numInScatteringPoints; ++i)
    {
        float sunRayLength = distThroughAtmosphere(inScatterPoint, dirToSun) - epsilon;
        // Optical depth of a ray that reaches the current scatter point
        float sunRayOpticalDepth = opticalDepth(inScatterPoint, dirToSun, sunRayLength);
        // Optical depth from the scatter point to the camera
        float viewRayOpticalDepth = opticalDepth(inScatterPoint, -rayDir, rayStepSize * i);
        vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * scatterCoeffs / atmosphereHeight);

        float localDensity = densityAtHeight(sqrt(dot(inScatterPoint, inScatterPoint)));

        inScatteredLight += localDensity * transmittance * scatterCoeffs * rayStepSize / atmosphereHeight;
        inScatterPoint += rayStepSize * rayDir;
    }

    return inScatteredLight;
}

vec3 sunDiskColor(float phi, float theta)
{
    // Compute the angular distance between two points in a sphere
    float angDist = acos(sin(sunTheta) * sin(theta) + cos(sunTheta) * cos(theta) * cos(phi - sunPhi));

    // Sun core (bright circle)
    float sunCore = smoothstep(sunAngularSize, sunAngularSize * 0.8, angDist);

    float bloom1 = exp(-angDist * 16.0);
    float bloom2 = exp(-angDist * 12.0) * 0.25;
    float bloom3 = exp(-angDist * 8.0) * 0.0625;
    float scaledBloomIntensity = bloomIntensity * (1 + sin(sunTheta));
    float totalBloom = scaledBloomIntensity * (bloom1 + bloom2 + bloom3);
    
    vec3 color = sunColor * sunCore + sunBloomColor * totalBloom;

    return color;
}

void main()
{
    float x = TexCoords.x;
    float y = TexCoords.y;
    float z = TexCoords.z;

    // Compute the angles (phi goes around the vertical axis)
    //  - phi goes around the vertical axis
    //  - theta = pi/2 at the north pole, -pi/2 at the south pole
    float phi = atan(z , x);
    float r2 = sqrt(x*x + z*z);
    float theta = atan(y, r2);

    float scatterR = pow(400. / wavelengths.r, 4.) * scatteringStrength;
    float scatterG = pow(400. / wavelengths.g, 4.) * scatteringStrength;
    float scatterB = pow(400. / wavelengths.b, 4.) * scatteringStrength;
    vec3 scatterCoeffs = vec3(scatterR, scatterG, scatterB);

    vec3 color = calculateLight(cameraPos.y, phi, theta, scatterCoeffs);

    // Draw the Sun with bloom
    color += sunDiskColor(phi, theta);

    FragColor = vec4(color.r, color.g, color.b, 1.);

    // gl_FragDepth = 1;
}
