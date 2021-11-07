#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    // In this case, the texture coordinates are given only by the position, because
    // the cube is 1x1x1
    // Notice that these coordinates are a vec3
    TexCoords = aPos;    

    vec4 pos = projection * view * vec4(aPos, 1.0);

    // Set the z coordinate to be equal to w, which is 1 (this maximum). This way,
    // the skybox will always be behind all objects in the scene, useful for depth
    // testing
    gl_Position = pos.xyww;
}
