#version 410 core

// Shader used to draw the texture attachment of a FBO to the screen, after performing
// some postprocessing

out vec4 FragColor;

in vec2 TexCoords;

// Color data in the screen texture buffer, after the lighting pass and adding
// the skymap
uniform sampler2D screenTexture;
// Color of the fog
uniform vec3 fogColor;
// Position and depth output of the fragment shader
uniform sampler2D gPosition;

// Simple linear fog computation
float getFogFactorLinear(float d)
{
    // Values between which the fog effect appears
    const float FogMax = 400.0;
    // const float FogMin = 40.0;
    const float FogMin = 100.0;

    return clamp(1. - (FogMax - d) / (FogMax - FogMin), 0., 1.);
}

// Exponential fog computation
float getFogFactorExponential(float d)
{
    // Parameters for the fog
    const float density = 0.01;
    // const float gradient = 1.5;
    const float gradient = 20.;

    return clamp( 1. - exp(-pow((d * density), gradient)), 0., 1. );
}

void main()
{
    // Input color in the screen
    vec4 inputColor = texture(screenTexture, TexCoords);

    // Get the depth value from the position texture of the G-buffer
    float Depth = texture(gPosition, TexCoords).a;
    // Compute the fog factor from this value of the depth
    float fogFactor = getFogFactorExponential(Depth);

    // Mix the screen and fog colors with the factor
    FragColor = mix(inputColor, vec4(fogColor, 1.), fogFactor);
    
    // Apply gamma correction
    float gamma = 2.2;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}
