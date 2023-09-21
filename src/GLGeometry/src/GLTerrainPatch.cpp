#include "GLTerrainPatch.h"
#include "utils.h"

namespace GLGeometry
{
    //==============================
    // Methods of GLTerrainPatch
    //==============================

    // Constructor
    GLTerrainPatch::GLTerrainPatch()
    {

    }
    GLTerrainPatch::GLTerrainPatch( unsigned char* data, int width, int height, 
                                    int nChannels )
    {
        // Generate the vertices
        std::vector<float> vertices;
        for(unsigned int i = 0; i < height; i++)
        {
            for(unsigned int j = 0; j < width; j++)
            {
                // Retrieve texel for (i,j) tex coord
                unsigned char* texel = data + (j + width * i) * nChannels;
                // Raw height at coordinate
                unsigned char y = texel[0];

                Vertex thisVertex;
                thisVertex.Position = glm::vec3( -0.5f * height + i,
                                                 (int)y * 0.25f + 16,
                                                 -0.5f * width + j );
                thisVertex.Normal = glm::vec3( 0.f, 1.f, 0.f );
                thisVertex.TexCoords = glm::vec2( (float)i / width, (float)j / height );
                mVertices.push_back(thisVertex);
            }
        }

        // Indices of the vertices for the EBO
        for(unsigned int i = 0; i < height-1; i++)       // for each row a.k.a. each strip
        {
            for(unsigned int j = 0; j < width; j++)      // for each column
            {
                for(unsigned int k = 0; k < 2; k++)      // for each side of the strip
                {
                    mIndices.push_back(j + width * (i + k));
                }
            }
        }

        // Number of strips and vertices per strip
        mNumStrips = height - 1;
        mNumVertsPerStrip = width * 2;

        // Bind the VAO and the VBO (as a vertex buffer)
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        // Add the data to the VBO
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), 
                     &mVertices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        // Vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // Vertex texture coordinates
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // Vertex texture index
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexIndex));

        // Create the Element buffer object
        glGenBuffers(1, &mEBO);
        // Bind the EBO as an element array buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        // Add the data to the EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int),
                     &mIndices[0], GL_STATIC_DRAW);

        // Unbind the VAO
        glBindVertexArray(0);
    }

    // Function to render
    void GLTerrainPatch::draw()
    {
        // Disable face culling for drawing the terrain
        glDisable(GL_CULL_FACE);
        glBindVertexArray(mVAO);
        // render the mesh triangle strip by triangle strip - each row at a time
        for(unsigned int strip = 0; strip < mNumStrips; ++strip)
        {
            glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                           mNumVertsPerStrip,   // number of indices to render
                           GL_UNSIGNED_INT,     // index data type
                           (void*)(sizeof(unsigned int) * mNumVertsPerStrip * strip) ); // offset of the starting index
        }
        // Enable face culling again
        glEnable(GL_CULL_FACE);
    }


    //==============================
    // Methods of GLTerrainTessellated
    //==============================

    // Constructor
    GLTerrainTessellated::GLTerrainTessellated( int width, int height ) :
        mNrVertsPerPatch { 4 }, mNrPatches { 20 }
    {
        // Generate the vertices
        std::vector<float> vertices;

        for(unsigned i = 0; i < mNrPatches; i++)
        {
            for(unsigned j = 0; j < mNrPatches; j++)
            {
                vertices.push_back(-width/2.0f + width*i/(float)mNrPatches); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-height/2.0f + height*j/(float)mNrPatches); // v.z
                vertices.push_back(i / (float)mNrPatches); // u
                vertices.push_back(j / (float)mNrPatches); // v

                vertices.push_back(-width/2.0f + width*(i+1)/(float)mNrPatches); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-height/2.0f + height*j/(float)mNrPatches); // v.z
                vertices.push_back((i+1) / (float)mNrPatches); // u
                vertices.push_back(j / (float)mNrPatches); // v

                vertices.push_back(-width/2.0f + width*i/(float)mNrPatches); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-height/2.0f + height*(j+1)/(float)mNrPatches); // v.z
                vertices.push_back(i / (float)mNrPatches); // u
                vertices.push_back((j+1) / (float)mNrPatches); // v

                vertices.push_back(-width/2.0f + width*(i+1)/(float)mNrPatches); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-height/2.0f + height*(j+1)/(float)mNrPatches); // v.z
                vertices.push_back((i+1) / (float)mNrPatches); // u
                vertices.push_back((j+1) / (float)mNrPatches); // v
            }
        }

        // Bind the VAO and the VBO (as a vertex buffer)
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        // Pass the vertices as data to the VBO
        glBufferData( GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), 
                      &vertices[0], GL_STATIC_DRAW );
        // Vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        // Vertex texture coordinates
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));

        // Unbind the VAO
        glBindVertexArray(0);
    }

    // Function to render
    void GLTerrainTessellated::draw()
    {
        // Disable face culling for drawing the terrain
        glDisable(GL_CULL_FACE);

        // Set the number of vertices per patch
        glPatchParameteri(GL_PATCH_VERTICES, mNrVertsPerPatch);

        // Draw the terrain
        glBindVertexArray( mVAO );
        glDrawArrays( GL_PATCHES, 0, 4*mNrPatches*mNrPatches );

        // Enable face culling again
        glEnable(GL_CULL_FACE);
    }

}
