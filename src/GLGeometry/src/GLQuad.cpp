#include "GLGeometry.h"

using namespace GLBase;

namespace GLGeometry
{
    // Constructor
    GLQuad::GLQuad() : mEBO { 0 }, mIndices(6)
    {
        // Create the Element buffer object
        glGenBuffers(1, &mEBO);

        // Compute the four vertices by hand
        float vertices[] {  0.5f,  0.5f, 0.0f, 0.f, 0.f, 1.f, 1.f, 1.f, // top right
                            0.5f, -0.5f, 0.0f, 0.f, 0.f, 1.f, 1.f, 0.f, // bottom right
                           -0.5f, -0.5f, 0.0f, 0.f, 0.f, 1.f, 0.f, 1.f, // bottom left
                           -0.5f,  0.5f, 0.0f, 0.f, 0.f, 1.f, 0.f, 0.f  // top left
                         };
        for (int i = 0; i < 4; ++i)
        {
            Vertex thisVertex;
            thisVertex.Position = glm::vec3(vertices[i * 8 + 0],
                                            vertices[i * 8 + 1],
                                            vertices[i * 8 + 2]);
            thisVertex.Normal = glm::vec3(vertices[i * 8 + 3],
                                          vertices[i * 8 + 4],
                                          vertices[i * 8 + 5]);
            thisVertex.TexCoords = glm::vec2(vertices[i * 8 + 6],
                                             vertices[i * 8 + 7]);
            mVertices.push_back(thisVertex);
        }

        // Indices of the vertices for the EBO
        unsigned int indices[] { 3, 1, 0, // First triangle
                                 3, 2, 1  // Second triangle
                               };
        memcpy(&mIndices[0], &indices[0], 6 * sizeof(int));

        // Bind the VAO and the VBO (as a vertex buffer)
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        // Add the data to the VBO
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), 
                     &mVertices[0], GL_STATIC_DRAW);

        // Bind the EBO as an element array buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        // Add the data to the EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int),
                     &mIndices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // Vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // Vertex texture coordinates
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // Vertex texture index
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexIndex));

        // Unbind the VAO
        glBindVertexArray(0);
    }

    // Function to render
    void GLQuad::draw()
    {
        // Disable face culling for drawing the plane
        glDisable(GL_CULL_FACE);
        // Draw the quad
        glBindVertexArray(mVAO); // This also binds the corresponding EBO
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        // Enable face culling again
        glEnable(GL_CULL_FACE);
    }
}
