#version 410 core

#define M_PI 3.1415926535897932384626433832795

in vec3 TexCoords;
out vec4 FragColor;

uniform vec3 cameraPos;

float value = 0.5;

float earthRadius = 6.378E6;
float atmosphereHeight = 1.E6;
float athmosphereRadius = earthRadius + atmosphereHeight;
float athmosphereRadiusSq = athmosphereRadius * athmosphereRadius;

// float earthRadius = 6.378E1;
// float atmosphereHeight = 1.E1;

float distThroughAtmosphere(float height, float theta)
{
    float sinTheta = sin(theta);
    float yPrime = earthRadius + height;

    float yPrimeSq = yPrime * yPrime;
    float returnVal = - yPrime * sinTheta + sqrt(yPrimeSq * sinTheta*sinTheta - yPrimeSq + athmosphereRadiusSq);

    return returnVal / athmosphereRadius;
}

void main()
{
    float x = TexCoords.x;
    float y = TexCoords.y;
    float z = TexCoords.z;

    // Compute the angles (phi goes around the vertical axis)
    float phi = atan(z , x);
    float r2 = sqrt(x*x + z*z);
    float theta = atan(y, r2);
    // FragColor = vec4((phi + M_PI) / (2*M_PI), 0., (theta + M_PI / 2) / (M_PI), 1.);

    // // FragColor = vec4(0., y / 2 + 0.5, 0., 1.);
    // FragColor = vec4(x / 2 + 0.5, 0., 0., 1.);

    // // TODO Take into account camera position
        // The height above ground is given by the cameraPos.y
    // // TODO Take into account sun position

    float val = distThroughAtmosphere(cameraPos.y, theta);
    FragColor = vec4(val, val, val, 1.);

    // gl_FragDepth = 1;
}
