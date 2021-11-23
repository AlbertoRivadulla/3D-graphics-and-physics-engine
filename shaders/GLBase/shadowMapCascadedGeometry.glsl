#version 420 core

// Invocations must be equal to the number of levels in the cascaded shadow map
// This means that the shader will be run 5 times in parallel, and we can get the
// index of the current invocation with gl_InvocationID
layout (triangles, invocations = 5) in;
layout (triangle_strip, max_vertices = 3) out;

layout (std140, binding = 0) uniform LightSpaceMatrices
{
    // This allows for a maximum of 8 cascades
    mat4 lightSpaceMatrices[8];
};

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        // Layer of the array that we are writing to
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}
