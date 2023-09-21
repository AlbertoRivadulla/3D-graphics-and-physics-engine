#include "GLGeometry.h"

using namespace GLBase;

namespace GLGeometry
{
    // Constructor
    GLCubemap::GLCubemap(const std::string& texturesPath, const std::string& vertexShaderPath,
                      const std::string& fragmentShaderPath) :
        mShader(vertexShaderPath, fragmentShaderPath)
    {
        // Load the textures
        loadCubemap(texturesPath);

        // Pass the cubemap texture to the shader
        mShader.use();
        mShader.setInt("skybox", 0);

        // Setup the screen quad
        setupScreenQuad();
    }
    // Constructor without textures
    GLCubemap::GLCubemap(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) :
        mShader(vertexShaderPath, fragmentShaderPath)
    {
        // No textures to load, if no path for them is given

        // Setup the screen quad
        setupScreenQuad();
    }

    // Setup the screen quad
    void GLCubemap::setupScreenQuad()
    {
        // Vertices of the screen quad
        float screenQuadVertices[] = {
             // positions  // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        // Generate the VAO and VBO
        glGenVertexArrays(1, &mScreenVAO);
        glGenBuffers(1, &mScreenVBO);
        glBindVertexArray(mScreenVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mScreenVBO);
        // Set the data in the VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), &screenQuadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    void GLCubemap::setViewProjection(glm::mat4& view, glm::mat4& projection)
    {
        // Pass the inverse of the view and projection matrices to the shader
        // In the view matrix, remove the translation components
        mShader.use();
        mShader.setMat4("viewInv", glm::inverse(glm::mat4(glm::mat3(view))));
        mShader.setMat4("projectionInv", glm::inverse(projection));
    }

    void GLCubemap::draw()
    {
        // Change face culling, since we are drawing the cube from inside
        // glCullFace(GL_FRONT);

        // The depth buffer of the skybox is filled with 1's, so we need to change 
        // the depth function from LESS to LEQUAL
        // glDepthFunc(GL_LEQUAL);
        
        // Activate the shader
        mShader.use();
        // Activate the skybox texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemapTexture);

        // Draw the skybox quad
        glBindVertexArray(mScreenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Set again the depth function to LESS, and enable face culling
        // glDepthFunc(GL_LESS);
        // glCullFace(GL_BACK);
    }

    // Function to render a flat sky
    void GLCubemap::drawFlat()
    {
        // Change face culling, since we are drawing the cube from inside
        // glCullFace(GL_FRONT);

        // The depth buffer of the skybox is filled with 1's, so we need to change 
        // the depth function from LESS to LEQUAL
        // glDepthFunc(GL_LEQUAL);
        
        // Activate the shader
        mShader.use();

        // Draw the skybox quad
        glBindVertexArray(mScreenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Set again the depth function to LESS, and enable face culling
        // glDepthFunc(GL_LESS);
        // glCullFace(GL_BACK);
    }

    void GLCubemap::loadCubemap(const std::string& texturesPath)
    {
        // List of the sides of the cubemap
        std::vector<std::string> sides { "right", "left", "top", "bottom", "front",
                                         "back" };
        // Path of each side
        std::vector<std::string> sidesPaths(6);
        for (int i = 0; i < 6; ++i)
        {
            sidesPaths[i] = texturesPath + "/" + sides[i] + ".jpg";
        }

        // Create a texture and bind it to GL_TEXTURE_CUBE_MAP
        glGenTextures(1, &mCubemapTexture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemapTexture);

        // Load the texture for each face of the cube.
        // Start with GL_TEXTURE_CUBE_MAP_POSITIVE_X (right face), and increment it 
        // by one for each step. The order is:
        //  right - left - top - bottom - back - front
        int width, height, nrChannels;
        unsigned char* data;
        // The textures should not be flipped, since cubemap images are expected to 
        // start at the top left, instead of the bottom left.
        stbi_set_flip_vertically_on_load(false);
        // Iterate over the faces
        for (int i = 0; i < sidesPaths.size(); ++i)
        {
            // Load the data in each face
            data = stbi_load(sidesPaths[i].c_str(), &width, &height, &nrChannels, 0);
            // Generate the texture from the loaded data, in the corresponding face
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                         0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free( data );
        }
        // Specify the wrapping and filtering methods for the cubemap texture.
        // The R dimension corresponds to the tird dimension, z.
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}
