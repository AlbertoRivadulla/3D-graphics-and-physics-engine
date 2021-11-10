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
            GLCubemap(const std::string& texturesPath = "../resources/textures/skybox", 
                      const char* vertexShaderPath = "../shaders/GLGeometry/skyboxVertex.glsl",
                      const char* fragmentShaderPath = "../shaders/GLGeometry/skyboxFragment.glsl");

            // Method to pass the view and projection matrices to the shader
            void setViewProjection(glm::mat4& view, glm::mat4& projection);

            // Function to render
            void draw();

        private:
            // Cube geometry
            GLCube mCube;

            // Cubemap texture
            unsigned int mCubemapTexture;

            // Shader
            Shader mShader;

            // Method to load a cubemap from a file
            void loadCubemap(const std::string& texturesPath);
    };
}

#endif
