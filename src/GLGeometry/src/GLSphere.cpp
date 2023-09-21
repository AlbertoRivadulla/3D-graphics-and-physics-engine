#include "GLGeometry.h"

namespace GLGeometry
{
    // Constructor
    GLSphere::GLSphere(int nrVertices) : 
        mNrVertices { nrVertices + 1 }
    {
        // float pi = glm::pi<float>();

        // Create the Element buffer object
        glGenBuffers(1, &mEBO);

        // Lists of mVertices and mIndices
        mVertices.reserve(2 * (mNrVertices * mNrVertices - mNrVertices + 1));
        mIndices.reserve(3 * 4 * mNrVertices * (mNrVertices - 1));

        // mVertices.reserve(2 * (mNrVertices * mNrVertices + 1));
        // mIndices.reserve(3 * 4 * mNrVertices * (mNrVertices));

        // Generate the mVertices
        Vertex thisVertex;
        // North pole
        thisVertex.Position = glm::vec3(0., 0.5, 0.);
        thisVertex.Normal = glm::vec3(0., 0.5, 0.);
        thisVertex.TexCoords = glm::vec2(0.5, 0.99);
        // thisVertex.TexCoords = glm::vec2(0., 0.);
        mVertices.push_back(thisVertex);
        // Loop vertically
        for (int t = 1; t < mNrVertices; ++t)
        {
            // Loop horizontally
            for (int p = 0; p < 2 * mNrVertices; ++p)
            {
                float x { 0.5f * glm::sin(glm::pi<float>() * (float)t / mNrVertices) *
                                         glm::cos(glm::pi<float>() * (float)p / (mNrVertices - 0.5f) ) };
                float y { 0.5f * glm::cos(glm::pi<float>() * (float)t / mNrVertices) };
                float z { 0.5f * glm::sin(glm::pi<float>() * (float)t / mNrVertices) *
                                         glm::sin(glm::pi<float>() * (float)p / (mNrVertices - 0.5f) ) };

                thisVertex.Position = glm::vec3(x, y, z);
                thisVertex.Normal = glm::vec3(x, y, z);
                // Texture coordinates from a cylindrical projection
                thisVertex.TexCoords = glm::vec2( p / ( 2.f*mNrVertices - 1.f ),
                                                  0.99 * ( y + 0.5f )
                                                );

                mVertices.push_back(thisVertex);
            }
        }
        // South pole
        thisVertex.Position = glm::vec3(0., -0.5, 0.);
        thisVertex.Normal = glm::vec3(0., -0.5, 0.);
        thisVertex.TexCoords = glm::vec2(0.5, 0.);
        mVertices.push_back(thisVertex);

        // Generate the mIndices for the EBO
        // Join the upper cusp
        for (int p = 1; p < 2 * mNrVertices + 1; ++p)
        {
            mIndices.push_back(0);
            mIndices.push_back(p % (2 * mNrVertices) + 1);
            mIndices.push_back(p);
        }
        // Loop horizontally
        // for (int p = 0; p < 2 * mNrVertices; ++p)
        for (int p = 0; p < 2 * mNrVertices; ++p)
        {
            // Loop vertically
            for (int t = 0; t < mNrVertices - 2; ++t)
            {
                // Triangles pointing down
                mIndices.push_back(p + t * (2 * mNrVertices) + 1);
                mIndices.push_back((p + 1) % (2 * mNrVertices) + t * (2 * mNrVertices) + 1);
                mIndices.push_back(p + (t + 1) * (2 * mNrVertices) + 1);

                // Triangles pointing up
                mIndices.push_back((p + 1) % (2 * mNrVertices) + t * (2 * mNrVertices) + 1);
                mIndices.push_back((p + 1) % (2 * mNrVertices) + (t + 1) * (2 * mNrVertices) + 1);
                mIndices.push_back(p + (t + 1) * (2 * mNrVertices) + 1);
            }
        }
        // Join the lower cusp
        for (int p = 0; p < 2 * mNrVertices; ++p)
        {
            // Vertical lines
            mIndices.push_back(2 * (mNrVertices * mNrVertices - mNrVertices + 1) - 1);
            mIndices.push_back(p + 1 + (mNrVertices - 2) * 2 * mNrVertices);
            mIndices.push_back((p + 1) % (2 * mNrVertices) + 1 + (mNrVertices - 2) * 2 * mNrVertices);
        }

        // // Generate the mIndices for the EBO
        // // Join the upper cusp
        // for (int p = 1; p < 2 * mNrVertices + 2; ++p)
        // {
        //     mIndices.push_back(0);
        //     mIndices.push_back(p % (2 * mNrVertices) + 1);
        //     mIndices.push_back(p);
        // }
        // // Loop horizontally
        // // for (int p = 0; p < 2 * mNrVertices; ++p)
        // for (int p = 0; p < 2 * mNrVertices; ++p)
        // {
        //     // Loop vertically
        //     for (int t = 0; t < mNrVertices - 2; ++t)
        //     {
        //         // Triangles pointing down
        //         mIndices.push_back(p + t * (2 * mNrVertices) + 1);
        //         mIndices.push_back((p + 1) % (2 * mNrVertices) + t * (2 * mNrVertices) + 1);
        //         mIndices.push_back(p + (t + 1) * (2 * mNrVertices) + 1);
        //
        //         // Triangles pointing up
        //         mIndices.push_back((p + 1) % (2 * mNrVertices) + t * (2 * mNrVertices) + 1);
        //         mIndices.push_back((p + 1) % (2 * mNrVertices) + (t + 1) * (2 * mNrVertices) + 1);
        //         mIndices.push_back(p + (t + 1) * (2 * mNrVertices) + 1);
        //     }
        // }
        // // Join the lower cusp
        // for (int p = 0; p < 2 * mNrVertices; ++p)
        // {
        //     // Vertical lines
        //     mIndices.push_back(2 * (mNrVertices * mNrVertices - mNrVertices + 1) - 1);
        //     mIndices.push_back(p + 1 + (mNrVertices - 2) * 2 * mNrVertices);
        //     mIndices.push_back((p + 1) % (2 * mNrVertices) + 1 + (mNrVertices - 2) * 2 * mNrVertices);
        // }

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
    void GLSphere::draw()
    {
        // Draw the quad
        glBindVertexArray(mVAO); // This also binds the corresponding EBO
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
