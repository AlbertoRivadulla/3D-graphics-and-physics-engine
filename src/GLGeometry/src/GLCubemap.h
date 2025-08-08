#ifndef GLCUBEMAP_H
#define GLCUBEMAP_H

#include "GLGeometry.h"

using namespace GLBase;

namespace GLGeometry
{
    class GLCubemap : public GLObject
    {
        public:
            // Constructor
            // The default path for the textures is
            //      ../resources/textures/skybox
            GLCubemap(bool hasTexture,
                      const std::string& texturesPath, 
                      const std::string& vertexShaderPath = std::string(BASE_DIR_SHADERS) + "/GLGeometry/skyboxVertex.glsl",
                      const std::string& fragmentShaderPath = std::string(BASE_DIR_SHADERS) + "/GLGeometry/skyboxFragment.glsl");
            // Constructor without textures
            GLCubemap(const std::string& vertexShaderPath = std::string(BASE_DIR_SHADERS) + "/GLGeometry/skyboxVertex.glsl",
                      const std::string& fragmentShaderPath = std::string(BASE_DIR_SHADERS) + "/GLGeometry/skyboxFragmentFlat.glsl");

            // Setup the screen quad
            void setupScreenQuad();

            // Method to pass the view and projection matrices to the shader
            void setViewProjection(glm::mat4& view, glm::mat4& projection);

            // Function to render
            void draw();

        private:
            // VAO and VBO for the screen quad
            unsigned int mScreenVAO;
            unsigned int mScreenVBO;

            // Cubemap texture
            bool mHasTexture;
            unsigned int mCubemapTexture;

            // Shader
            Shader mShader;

            // Method to load a cubemap from a file
            void loadCubemap(const std::string& texturesPath);

        // --------------------------------------------------------------------
        // The following two functions are not used, but need to be implemented
        // as they are declared as virtual in its parent class GLObject
        public:
            // Function to set the model matrix
            void setModelMatrix(const glm::vec3& translation, const float& rotationAngle, 
                                const glm::vec3& rotationAxis, const glm::vec3& scale)
            {}

            // Function to read the model matrix
            glm::mat4 getModelMatrix()
            {
                return glm::mat4( 0.f );
            }
    };
}

#endif
