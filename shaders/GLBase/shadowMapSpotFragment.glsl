#version 410 core

in vec4 fragPos;
in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    // Distance from the fragment to the light, mapped to the range [0, 1]
    gl_FragDepth = length(fragPos.xyz - lightPos.xyz) / farPlane;
}
