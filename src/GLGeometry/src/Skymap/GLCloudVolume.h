#ifndef GLCLOUDVOLUME_H
#define GLCLOUDVOLUME_H

#include "GLGeometry.h"

using namespace GLBase;

namespace GLGeometry
{
    class GLCloudVolume
    {
        public:
            void setupNoiseShader(
                const std::string& vertexShaderPath = std::string(BASE_DIR_SHADERS) + "/GLGeometry/sky/cloudTextureVertex.glsl",
                const std::string& fragmentShaderPath = std::string(BASE_DIR_SHADERS) + "/GLGeometry/sky/cloudTextureFragment.glsl"
            );

            void setupVolumeShader(
                const std::string& vertexShaderPath = std::string(BASE_DIR_SHADERS) + "/GLGeometry/sky/cloudVolumeVertex.glsl",
                const std::string& fragmentShaderPath = std::string(BASE_DIR_SHADERS) + "/GLGeometry/sky/cloudVolumeFragment.glsl"
            );

            void setViewProjection(glm::mat4& view, glm::mat4& projection);
            void setCameraPosition(glm::vec3& cameraPosition);
            void setSunPosition(float phi, float theta);

            void computeNoise();

            void draw();
        private:
            Shader mNoiseShader;
            Shader mCloudVolumeShader;

            // VAO and VBO for the screen quad
            unsigned int mScreenVAO;
            unsigned int mScreenVBO;

            // TODO: Variables for the noise texture, quad, VAO, etc

            void setupScreenQuad();
    };
} // GLGeometry

#endif
