#include "GLGeometry.h"

using namespace GLBase;

namespace GLGeometry
{
    // Constructor
    GLCubemap::GLCubemap(const std::string& texturesPath, const char* vertexShaderPath,
                      const char* fragmentShaderPath) :
        mShader(vertexShaderPath, fragmentShaderPath) , mCube()
    {
        // Load the textures
        loadCubemap(texturesPath);

        // Pass the cubemap texture to the shader
        mShader.use();
        mShader.setInt("skybox", 0);
    }
    // Constructor without textures
    GLCubemap::GLCubemap(const char* vertexShaderPath, const char* fragmentShaderPath) :
        mShader(vertexShaderPath, fragmentShaderPath) , mCube()
    {
        // No textures to load, if no path for them is given
    }

    void GLCubemap::setViewProjection(glm::mat4& view, glm::mat4& projection)
    {
        // In the view matrix, remove the translation components
        mShader.setMat4("view", glm::mat4(glm::mat3(view)));
        mShader.setMat4("projection", projection);
    }

    void GLCubemap::draw()
    {
        // Disable face culling, since we are drawing the cube from inside
        // glDisable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        // The depth buffer of the skybox is filled with 1's, so we need to change 
        // the depth function from LESS to LEQUAL
        glDepthFunc(GL_LEQUAL);
        
        // Activate the shader
        mShader.use();
        // Activate the skybox texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mCubemapTexture);
        // Draw the skybox cube
        mCube.draw();

        // Set again the depth function to LESS, and enable face culling
        glDepthFunc(GL_LESS);
        // glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    // Function to render a flat sky
    void GLCubemap::drawFlat()
    {
        // Disable face culling, since we are drawing the cube from inside
        // glDisable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        // The depth buffer of the skybox is filled with 1's, so we need to change 
        // the depth function from LESS to LEQUAL
        glDepthFunc(GL_LEQUAL);
        
        // Activate the shader
        mShader.use();
        // Draw the skybox cube
        mCube.draw();

        // Set again the depth function to LESS, and enable face culling
        glDepthFunc(GL_LESS);
        // glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
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
