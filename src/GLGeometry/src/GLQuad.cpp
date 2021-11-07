#include "GLGeometry.h"

using namespace GLBase;

namespace GLGeometry
{
    // Constructor
    GLQuad::GLQuad() : mVertices(4)
    {
        // Compute the four vertices by hand
        float vertices[] {  0.5f,  0.5f, 0.0f, 0.f, 0.f, 1.f, 1.f, 1.f, // top right
                            0.5f, -0.5f, 0.0f, 0.f, 0.f, 1.f, 1.f, 0.f, // bottom right
                           -0.5f, -0.5f, 0.0f, 0.f, 0.f, 1.f, 0.f, 1.f, // bottom left
                           -0.5f,  0.5f, 0.0f, 0.f, 0.f, 1.f, 0.f, 0.f  // top left
                         };
        for (int i = 0; i < 4; ++i)
        {
            Vertex thisVertex;
            thisVertex.Position = glm::vec3(vertices[i * sizeof(Vertex) + 0],
                                            vertices[i * sizeof(Vertex) + 1],
                                            vertices[i * sizeof(Vertex) + 2]);
            thisVertex.Normal = glm::vec3(vertices[i * sizeof(Vertex) + 3],
                                          vertices[i * sizeof(Vertex) + 4],
                                          vertices[i * sizeof(Vertex) + 5]);
            thisVertex.TexCoords = glm::vec2(vertices[i * sizeof(Vertex) + 6],
                                             vertices[i * sizeof(Vertex) + 7]);
            mVertices.push_back(thisVertex);
        }

        // Indices of the vertices for the EBO
        unsigned int indices[] { 0, 1, 3, // First triangle
                                 1, 2, 3  // Second triangle
                               };

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

        // Unbind the VAO
        glBindVertexArray(0);
    }

    // Function to render
    void GLQuad::draw()
    {
        // Draw the quad
        glBindVertexArray(mVAO); // This also binds the corresponding EBO
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Functions to set the VAO, VBO and EBO from a child class
    void GLQuad::setVAO()
    {

    }

    void GLQuad::setVBO()
    {

    }

    void GLQuad::setEBO()
    {

    }

}
