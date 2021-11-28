#include "GLGeometry.h"

using namespace GLBase;

namespace GLGeometry
{
    // Constructor
    GLCone::GLCone(int nrVerticesCircle) : 
        mNrVertices { nrVerticesCircle }
    {
        // Create the Element buffer object
        glGenBuffers(1, &mEBO);

        // Vectors of vertices and indices
        mVertices.reserve(2 * mNrVertices + 2); // +2 because of the vertices in the center of the circles
        mIndices.reserve(2 * 3 * mNrVertices);

        // Generate the vertices of the base
        for (int angle = 0; angle < mNrVertices; ++angle)
        {
            Vertex thisVertex;
            // Spatial coordinates, which are also the components of the normal
            float x { 0.5f * glm::cos(2.f * glm::pi<float>() * (float)angle / mNrVertices) };
            float z { 0.5f * glm::sin(2.f * glm::pi<float>() * (float)angle / mNrVertices) };
            thisVertex.Position = glm::vec3(x, -0.5, z);
            // Normal vector
            thisVertex.Normal = glm::vec3(0., -1., 0.);
            // Texture coordinates
            thisVertex.TexCoords = glm::vec2(x + 0.5, z + 0.5);
            // Texture index
            thisVertex.TexIndex = 1;

            mVertices.push_back(thisVertex);
        }
        // Add a vertex in the center of the circle
        Vertex thisVertex;
        thisVertex.Position = glm::vec3(0., -0.5, 0.);
        thisVertex.Normal = glm::vec3(0., -1., 0.);
        thisVertex.TexCoords = glm::vec2(0.5, 0.5);
        thisVertex.TexIndex = 1;
        mVertices.push_back(thisVertex);

        // Generate the vertices of the sides
        for (int angle = 0; angle < mNrVertices; ++angle)
        {
            Vertex thisVertex;
            // Spatial coordinates, which are also the components of the normal
            float x { 0.5f * glm::cos(2.f * glm::pi<float>() * (float)angle / mNrVertices) };
            float z { 0.5f * glm::sin(2.f * glm::pi<float>() * (float)angle / mNrVertices) };
            thisVertex.Position = glm::vec3(x, -0.5, z);
            // Normal vector
            thisVertex.Normal = glm::vec3(x, 0., z);
            // Texture coordinates
            thisVertex.TexCoords = glm::vec2(angle / (float)mNrVertices, 0);
            // Texture index
            thisVertex.TexIndex = 0;

            mVertices.push_back(thisVertex);
        }
        // Add a vertes in the cusp
        thisVertex.Position = glm::vec3(0., 0.5, 0.);
        // thisVertex.Normal = glm::vec3(0., 1., 0.);
        thisVertex.Normal = glm::vec3(0., 0., 0.);
        thisVertex.TexCoords = glm::vec2(0.5, 1.);
        thisVertex.TexIndex = 1;
        mVertices.push_back(thisVertex);

        // Compute the indices
        for (int i = 0; i < mNrVertices; ++i)
        {
            // Join the triangles in the base
            mIndices.push_back(i);
            mIndices.push_back((i + 1) % mNrVertices);
            mIndices.push_back(mNrVertices);

            // Join the triangles in the sides
            mIndices.push_back(i + mNrVertices + 1);
            mIndices.push_back(mNrVertices + mNrVertices + 1);
            mIndices.push_back((i + 1) % mNrVertices + mNrVertices + 1);
        }

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
    void GLCone::draw()
    {
        // Disable face culling for drawing the plane
        // glDisable(GL_CULL_FACE);

        // Draw the quad
        glBindVertexArray(mVAO); // This also binds the corresponding EBO
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Enable face culling again
        // glEnable(GL_CULL_FACE);
    }
}
