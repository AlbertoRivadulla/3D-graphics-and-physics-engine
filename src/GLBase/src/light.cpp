#include "light.h"

using namespace GLGeometry;

namespace GLBase
{
    //==============================
    // Methods of the Light base class
    //==============================

    // Constructor
    Light::Light(glm::vec3 color, glm::vec3 position, float intensity,
                 float attenLinear, float attenQuadratic, int shadowRes) :
        mColor { color }, mPosition { position },
        mIntensity { intensity }, mAttenLinear { attenLinear },
        mAttenQuadratic { attenQuadratic }, mShadowMapResolution { shadowRes }

    {
        // Configure the FBO and the texture for the shadowmap
        setupShadowMap();
    }

    // Method to configure the shadow map framebuffer and texture
    void Light::setupShadowMap()
    {
        // Generate the framebuffer and bind it
        glGenFramebuffers(1, &mShadowMapFBO);

        // Generate the texture
        glGenTextures(1, &mShadowMapTexture);
        glBindTexture(GL_TEXTURE_2D, mShadowMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
             mShadowMapResolution, mShadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Use the texture as the depth attachment of the FBO
        glBindFramebuffer(GL_FRAMEBUFFER, mShadowMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);  
    }

    //==============================
    // Methods of the DirectionalLight class
    //==============================

    // Constructor
    DirectionalLight::DirectionalLight(glm::vec3 color, glm::vec3 position, 
                                       glm::vec3 direction, float intensity, 
                                       float attenLinear, float attenQuadratic,
                                       int shadowRes) :
        mDirection { glm::normalize(direction) }, 
        Light( color, position, intensity, attenLinear, attenQuadratic, shadowRes )
    {
    }

    // Method to compute the light space matrix from a camera
    // Following https://learnopengl.com/Guest-Articles/2021/CSM
    void DirectionalLight::computeLightSpaceMatrix(const Camera& camera)
    {
        // Get the position of the eight corners of the frustum
        std::vector<glm::vec4> corners { camera.getFrustumCornersWorldSpace() };

        // Compute the center of the frustum
        glm::vec3 center { 0., 0., 0. };
        for (const auto& v : corners)
        {
            center += glm::vec3(v);
        }
        center /= corners.size();

        // Compute the light view matrix
        const glm::mat4 lightView { glm::lookAt( center - mDirection,       // Position of the light
                                                 center,                    // Center of the frustum 
                                                 glm::vec3(0., 1., 0.) ) }; // Up vector

        // Get the maximum and minimum coordinates of the corners of the frustum in
        // the light's coordinate system
        float minX { std::numeric_limits<float>::max() };
        float minY { std::numeric_limits<float>::max() };
        float minZ { std::numeric_limits<float>::max() };
        float maxX { std::numeric_limits<float>::min() };
        float maxY { std::numeric_limits<float>::min() };
        float maxZ { std::numeric_limits<float>::min() };
        for (const auto& v : corners)
        {
            // Project the current corner to light space
            const glm::vec4 vLightSpace { lightView * v };
            // Compare its coordinates to the ones stored
            minX = std::min(vLightSpace.x, minX);
            minY = std::min(vLightSpace.y, minY);
            minZ = std::min(vLightSpace.z, minZ);
            maxX = std::max(vLightSpace.x, maxX);
            maxY = std::max(vLightSpace.y, maxY);
            maxZ = std::max(vLightSpace.z, maxZ);
        }

        // Increase the size of the light frustum in the Z direction
        // constexpr float zMult { 10.f };
        constexpr float zMult { 1.1f };
        if (minZ < 0)
            minZ *= zMult;
        else
            minZ /= zMult;
        if (maxZ < 0)
            maxZ /= zMult;
        else
            maxZ *= zMult;

        // Compute the projection matrix of the light
        const glm::mat4 lightProjection { glm::ortho(minX, maxX, minY, maxY, 
                                                     minZ, maxZ) };

        // Compute the lightSpaceMatrix
        mLightSpaceMatrix = lightProjection * lightView;
    }

    // Method to compute the shadow map
    void DirectionalLight::computeShadowMap(const Shader& shader, const Camera& camera,
                                            const std::vector<GLElemObject*> objectsWithShadow)
    {
        // Compute the light space matrix
        computeLightSpaceMatrix(camera);

        // Change the size of the viewport
        glViewport(0, 0, mShadowMapResolution, mShadowMapResolution);

        // Bind the FBO, whose depth attachment is the shadow map texture
        glBindFramebuffer(GL_FRAMEBUFFER, mShadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Pass the lightSpaceMatrix to the shader (which was already bound before
        // this function call)
        shader.setMat4("lightSpaceMatrix", mLightSpaceMatrix);

        // // Set face culling to the front faces
        // glCullFace(GL_FRONT);
        // Draw each object in the scene
        for (auto object : objectsWithShadow)
        {
            // Set the model matrix of the object in the shader
            shader.setMat4("model", object->getModelMatrix());
            // Draw the object
            object->draw();
        }
        // // Restore face culling
        // glCullFace(GL_BACK);
    }

    // Method to pass the light to a shader
    void DirectionalLight::configureShader(const Shader& shader, unsigned int& indexDirectional, 
                                           unsigned int& indexSpot, unsigned int indexPoint,
                                           unsigned int& indexShadow) const
    {
        // Pass the light properties to the shader
        // The shader must be bound before calling this method
        shader.setVec3("dirLights[" + std::to_string(indexDirectional) + "].color", mColor);
        shader.setVec3("dirLights[" + std::to_string(indexDirectional) + "].position", mPosition);
        shader.setVec3("dirLights[" + std::to_string(indexDirectional) + "].direction", mDirection);

        shader.setFloat("dirLights[" + std::to_string(indexDirectional) + "].intensity", mIntensity);
        shader.setFloat("dirLights[" + std::to_string(indexDirectional) + "].kLinear", mAttenLinear);
        shader.setFloat("dirLights[" + std::to_string(indexDirectional) + "].kQuadratic", mAttenQuadratic);

        // // Bind the shadowmap texture to the corresponding texture unit
        // glActiveTexture(GL_TEXTURE0 + indexShadow);
        // glBindTexture(GL_TEXTURE_2D, mShadowMapTexture);

        // // Pass also the index of the texture unit for the shadowmap, and the 
        // // light space matrix
        // shader.setMat4("dirLights[" + std::to_string(indexDirectional) + "].lightSpaceMatrix", mLightSpaceMatrix);
        // shader.setInt("dirLights[" + std::to_string(indexDirectional) + "].shadowMap", indexShadow);

        // Increase the counter of the directiona lights
        indexDirectional++;
        // // Increase the counter of the shadow maps
        // indexShadow++;
    }

    // Method to pass the lightSpaceMatrix to a shader
    void DirectionalLight::configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                           unsigned int& indexSpot, unsigned int indexPoint,
                                           unsigned int& indexShadow) const
    {
        // Bind the shadowmap texture to the corresponding texture unit
        glActiveTexture(GL_TEXTURE0 + indexShadow);
        glBindTexture(GL_TEXTURE_2D, mShadowMapTexture);

        // Pass also the index of the texture unit for the shadowmap, and the 
        // light space matrix
        shader.setMat4("dirLights[" + std::to_string(indexDirectional) + "].lightSpaceMatrix", mLightSpaceMatrix);
        shader.setInt("dirLights[" + std::to_string(indexDirectional) + "].shadowMap", indexShadow);

        // Increase the counter of the directiona lights
        indexDirectional++;
        // Increase the counter of the shadow maps
        indexShadow++;
    }

    //==============================
    // Methods of the SpotLight class
    //==============================

    // Constructor
    SpotLight::SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, 
                         float angleInner, float angleOuter,
                         float intensity, float attenLinear, float attenQuadratic, 
                         int shadowRes) :
        mDirection { glm::normalize(direction) }, 
        Light( color, position, intensity, attenLinear, attenQuadratic, shadowRes )
    {
        // Compute the cosines of the two angles
        mCosAngleInner = glm::cos(glm::radians(angleInner));
        mCosAngleOuter = glm::cos(glm::radians(angleOuter));

        // Compute the maximum distance of influence of the light, where the intensity
        // becomes 5/256 times its value at d = 0
        float disc { mAttenLinear * mAttenLinear - 4.f * mAttenQuadratic * ( 1.f - mIntensity * 256.f / 5.f ) };
        mRadiusMax = ( - mAttenLinear + glm::sqrt(disc) ) / (2.f * mAttenQuadratic);

    }

    // Method to compute the shadow map
    void SpotLight::computeShadowMap(const Shader& shader, const Camera& camera,
                                     const std::vector<GLElemObject*> objectsWithShadow)
    {

    }

    // Method to pass the light to a shader
    void SpotLight::configureShader(const Shader& shader, unsigned int& indexDirectional, 
                                    unsigned int& indexSpot, unsigned int indexPoint,
                                    unsigned int& indexShadow) const
    {
        // Pass the light properties to the shader
        // The shader must be bound before calling this method
        shader.setVec3("spotLights[" + std::to_string(indexSpot) + "].color", mColor);
        shader.setVec3("spotLights[" + std::to_string(indexSpot) + "].position", mPosition);
        shader.setVec3("spotLights[" + std::to_string(indexSpot) + "].direction", mDirection);

        shader.setFloat("spotLights[" + std::to_string(indexSpot) + "].intensity", mIntensity);
        shader.setFloat("spotLights[" + std::to_string(indexSpot) + "].kLinear", mAttenLinear);
        shader.setFloat("spotLights[" + std::to_string(indexSpot) + "].kQuadratic", mAttenQuadratic);

        shader.setFloat("spotLights[" + std::to_string(indexSpot) + "].cosAngleInner", mCosAngleInner);
        shader.setFloat("spotLights[" + std::to_string(indexSpot) + "].cosAngleOuter", mCosAngleOuter);
        shader.setFloat("spotLights[" + std::to_string(indexSpot) + "].radiusMax", mCosAngleInner);
        shader.setFloat("pointLights[" + std::to_string(indexSpot) + "].radiusMax", mRadiusMax);

        // // Pass also the index of the texture unit for the shadowmap, and the 
        // // light space matrix
        // shader.setMat4("dirLights[" + std::to_string(indexDirectional) + "].lightSpaceMatrix", mLightSpaceMatrix);
        // shader.setInt("dirLights[" + std::to_string(indexDirectional) + "].shadowMap", indexShadow++);

        // Increase the counter of the spot lights
        indexSpot++;
        // // Increase the counter of the shadow maps
        // indexShadow++;
    }

    // Method to pass the lightSpaceMatrix to a shader
    void SpotLight::configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                           unsigned int& indexSpot, unsigned int indexPoint,
                                           unsigned int& indexShadow) const
    {
        // Bind the shadowmap texture to the corresponding texture unit
        glActiveTexture(GL_TEXTURE0 + indexShadow);
        glBindTexture(GL_TEXTURE_2D, mShadowMapTexture);

        // Pass also the index of the texture unit for the shadowmap, and the 
        // light space matrix
        shader.setMat4("dirLights[" + std::to_string(indexDirectional) + "].lightSpaceMatrix", mLightSpaceMatrix);
        shader.setInt("dirLights[" + std::to_string(indexDirectional) + "].shadowMap", indexShadow++);

        // Increase the counter of the spot lights
        indexSpot++;
        // Increase the counter of the shadow maps
        indexShadow++;
    }

    //==============================
    // Methods of the PointLight class
    //==============================

    // Constructor
    PointLight::PointLight(glm::vec3 color, glm::vec3 position, float intensity, 
                           float attenLinear, float attenQuadratic, int shadowRes) :
        Light( color, position, intensity, attenLinear, attenQuadratic, shadowRes )
    {
        // Compute the maximum distance of influence of the light, where the intensity
        // becomes 5/256 times its value at d = 0
        float disc { mAttenLinear * mAttenLinear - 4.f * mAttenQuadratic * ( 1.f - mIntensity * 256.f / 5.f ) };
        mRadiusMax = ( - mAttenLinear + glm::sqrt(disc) ) / (2.f * mAttenQuadratic);
        
    }

    // Method to compute the shadow map
    void PointLight::computeShadowMap(const Shader& shader, const Camera& camera,
                                      const std::vector<GLElemObject*> objectsWithShadow)
    {

    }

    // Method to pass the light to a shader
    void PointLight::configureShader(const Shader& shader, unsigned int& indexDirectional, 
                                     unsigned int& indexSpot, unsigned int indexPoint,
                                     unsigned int& indexShadow) const
    {
        // Pass the light properties to the shader
        // The shader must be bound before calling this method
        shader.setVec3("pointLights[" + std::to_string(indexPoint) + "].color", mColor);
        shader.setVec3("pointLights[" + std::to_string(indexPoint) + "].position", mPosition);

        shader.setFloat("pointLights[" + std::to_string(indexPoint) + "].intensity", mIntensity);
        shader.setFloat("pointLights[" + std::to_string(indexPoint) + "].kLinear", mAttenLinear);
        shader.setFloat("pointLights[" + std::to_string(indexPoint) + "].kQuadratic", mAttenQuadratic);
        shader.setFloat("pointLights[" + std::to_string(indexPoint) + "].radiusMax", mRadiusMax);

        // // Pass also the index of the texture unit for the shadowmap, and the 
        // // light space matrix
        // shader.setMat4("pointLights[" + std::to_string(indexDirectional) + "].lightSpaceMatrix", mLightSpaceMatrix);
        // shader.setInt("pointLights[" + std::to_string(indexDirectional) + "].shadowMap", indexShadow++);

        // Increase the count of the point lights
        indexPoint++;
        // // Increase the counter of the shadow maps
        // indexShadow++;
    }

    // Method to pass the lightSpaceMatrix to a shader
    void PointLight::configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                           unsigned int& indexSpot, unsigned int indexPoint,
                                           unsigned int& indexShadow) const
    {
        // Bind the shadowmap texture to the corresponding texture unit
        glActiveTexture(GL_TEXTURE0 + indexShadow);
        glBindTexture(GL_TEXTURE_2D, mShadowMapTexture);

        // Pass also the index of the texture unit for the shadowmap, and the 
        // light space matrix
        shader.setMat4("pointLights[" + std::to_string(indexDirectional) + "].lightSpaceMatrix", mLightSpaceMatrix);
        shader.setInt("pointLights[" + std::to_string(indexDirectional) + "].shadowMap", indexShadow++);

        // Increase the count of the point lights
        indexPoint++;
        // Increase the counter of the shadow maps
        indexShadow++;
    }

}
