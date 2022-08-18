#version 410 core
layout (location = 0) in vec3 aPos;

out vec4 fragPos;

/* out vec4 FragPos; */

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    fragPos = lightSpaceMatrix * model * vec4(aPos, 1.);
    gl_Position = fragPos;
}
