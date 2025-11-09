#include "GLGeometry.h"

using namespace GLBase;

namespace GLGeometry
{
    void GLCloudVolume::setupNoiseShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
    {
        mNoiseShader = Shader(vertexShaderPath, fragmentShaderPath);

        // TODO:

    }

    void GLCloudVolume::setupVolumeShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
    {
        mCloudVolumeShader = Shader(vertexShaderPath, fragmentShaderPath);

        setupScreenQuad();

        // TODO:

    }

    void GLCloudVolume::setViewProjection(glm::mat4& view, glm::mat4& projection)
    {
        // mCloudVolumeShader.use();
        // mCloudVolumeShader.setMat4("viewInv", glm::inverse(view));
        // mCloudVolumeShader.setMat4("projectionInv", glm::inverse(projection));

        // Pass the inverse of the view and projection matrices to the shader
        // In the view matrix, remove the translation components
        mCloudVolumeShader.use();
        mCloudVolumeShader.setMat4("viewInv", glm::inverse(glm::mat4(glm::mat3(view))));
        mCloudVolumeShader.setMat4("projectionInv", glm::inverse(projection));

        // TODO: Pass also the view matrix to the cloud texture/noise shader
    }

    void GLCloudVolume::setCameraPosition(glm::vec3& cameraPosition)
    {
        // // TODO:
        // mShader.use();
        // mShader.setVec3("cameraPos", cameraPosition);
    }

    void GLCloudVolume::setSunPosition(float phi, float theta)
    {
        // // TODO:
        // mShader.use();
        // mShader.setFloat("sunPhi", phi);
        // mShader.setFloat("sunTheta", theta);
    }

    void GLCloudVolume::computeNoise()
    {

    }

    void GLCloudVolume::draw()
    {
        // TODO: Use the position texture from the geometry pass

        mCloudVolumeShader.use();

        glEnable(GL_BLEND);
        // glBlendEquation(GL_FUNC_ADD);
        // glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw the skybox quad
        glBindVertexArray(mScreenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisable(GL_BLEND);
    }

    void GLCloudVolume::setupScreenQuad()
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

}
