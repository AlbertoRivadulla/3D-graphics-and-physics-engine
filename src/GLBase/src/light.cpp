#include "light.h"

using namespace GLGeometry;

namespace GLBase
{
    //==============================
    // Methods of the Light base class
    //==============================

    // Constructor
    Light::Light(glm::vec3 color, glm::vec3 position, float intensity,
                 float attenLinear, float attenQuadratic, int shadowRes,
                 LightType lightType) :
        mLightType { lightType },
        mColor { color }, mPosition { position },
        mIntensity { intensity }, mAttenLinear { attenLinear },
        mAttenQuadratic { attenQuadratic }, mShadowMapResolution { shadowRes }

    {
        // // Configure the FBO and the texture for the shadowmap
        // setupShadowMap();
    }

    //==============================
    // Methods of the DirectionalLight class
    //==============================

    // Constructor
    DirectionalLight::DirectionalLight(glm::vec3 color, glm::vec3 position, 
                                       glm::vec3 direction, float intensity, 
                                       float attenLinear, float attenQuadratic,
                                       int shadowRes, unsigned int nrShadowCascadeLevels) :
        mDirection { glm::normalize(direction) }, 
        mNrShadowCascadeLevels { nrShadowCascadeLevels },
        mLightSpaceMatrices(nrShadowCascadeLevels),
        mShadowCascadeDistances(nrShadowCascadeLevels + 1),
        Light( color, position, intensity, attenLinear, attenQuadratic, shadowRes,
               LIGHT_DIRECTIONAL )
    {
        // Set the near plane of the first frustum to -101, so we know that it needs
        // to be modified
        mShadowCascadeDistances[0] = -101.f;

        // Compute the up direction
        // Create a temporary vector, to compute the up direction as being perpendicular
        // to this and the direction vector
        glm::vec3 tempVector { 0., 1., 0. };
        // Check if this vector is parallel to the direction, and modify it in
        // that case
        if (fabs( glm::length( glm::cross(mDirection, tempVector) ) ) <= 0.001)
        {
            tempVector = glm::vec3( 1., 0., 0. );
        }
        // Compute the up direction
        mUpDirection = glm::cross(tempVector, mDirection);

        // Configure the FBO and the texture for the shadowmap
        setupShadowMap();
    }

    // Initialize the UBO for the light matrices
    unsigned int DirectionalLight::mLightMatricesUBO = 0;

    // Method to configure the shadow map framebuffer and texture
    void DirectionalLight::setupShadowMap()
    {
        // Generate the framebuffer
        glGenFramebuffers(1, &mShadowMapFBO);

        // Generate the array of textures, for cascaded shadow maps
        glGenTextures(1, &mShadowMapTexture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, mShadowMapTexture);
        glTexImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            GL_DEPTH_COMPONENT32F,
            mShadowMapResolution,
            mShadowMapResolution,
            mNrShadowCascadeLevels + 1,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // Configure the texture to be white outside the borders, where the light
        // does not produce shadows
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);  

        // Use the texture as the depth attachment of the FBO
        glBindFramebuffer(GL_FRAMEBUFFER, mShadowMapFBO);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowMapTexture, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mShadowMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        // Check if the framebuffer is complete
        int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            throw 0;
        }
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);  

        // Create a uniform buffer object for the light space matrices of each subfrustum
        if (mLightMatricesUBO == 0) // Check if this had already been done before
        {
            glGenBuffers(1, &mLightMatricesUBO);
            glBindBuffer(GL_UNIFORM_BUFFER, mLightMatricesUBO);
            glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
            // Bind this to the point 1, as in the corresponding shader
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, mLightMatricesUBO);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }

    // Method to compute the light space matrix from a camera
    // Following https://learnopengl.com/Guest-Articles/2021/CSM
    void DirectionalLight::computeLightSpaceMatrix(const Camera& camera, int index)
    {
        // Get the position of the eight corners of the frustum
        // Store also the z value of the near plane of the current subfrustum
        std::vector<glm::vec4> corners { camera.getFrustumCornersWorldSpace(mShadowCascadeDistances[index],
                                                                            mShadowCascadeDistances[index + 1]) };

        // Compute the center of the frustum
        glm::vec3 center { 0., 0., 0. };
        for (const auto& v : corners)
        {
            center += glm::vec3(v);
        }
        center /= corners.size();

        // Compute the light view matrix
        const glm::mat4 lightView { glm::lookAt( center - mDirection,          // Position of the light
                                                 center,                       // Center of the frustum 
                                                 mUpDirection ) }; // Up vector

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
        constexpr float zMult { 5.f };
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
        mLightSpaceMatrices[index] = lightProjection * lightView;
    }

    // Method to compute the shadow map
    void DirectionalLight::computeShadowMap(const Camera& camera,
                                            const std::vector<GLElemObject*> objectsWithShadow)
    {
        // Check if the frustums have not been computed yet
        if (mShadowCascadeDistances[0] < -100.)
        {
            // Get the near and far plane from the camera
            float zNear;
            float zFar;
            camera.getNearFarPlanes(zNear, zFar);

            // Set the position of the first and last planes
            mShadowCascadeDistances[0] = zNear;
            mShadowCascadeDistances[mNrShadowCascadeLevels] = zFar;

            // Compute the distances of the rest of the planes
            for (int i = mNrShadowCascadeLevels - 1; i > 0; --i)
            {
                mShadowCascadeDistances[i] = (mShadowCascadeDistances[i + 1] - zNear) / 2. + zNear;
            }
        }

        // Compute the differnt light space matrices
        for (int i = 0; i < mNrShadowCascadeLevels; ++i)
        {
            computeLightSpaceMatrix(camera, i);
        }

        // Pass the lightSpaceMatrices to the UBO
        // const auto lightMatrices = getLightSpaceMatrices();
        glBindBuffer(GL_UNIFORM_BUFFER, mLightMatricesUBO);
        for (size_t i = 0; i < mNrShadowCascadeLevels; ++i)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &mLightSpaceMatrices[i]);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Use the shader
        mShadowShader->use();

        // Bind the FBO, whose depth attachment is the shadow map texture
        glBindFramebuffer(GL_FRAMEBUFFER, mShadowMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_TEXTURE_2D_ARRAY, mShadowMapTexture, 0);
        // glBindTexture(GL_TEXTURE_2D_ARRAY, mShadowMapTexture);
        // Change the size of the viewport
        glViewport(0, 0, mShadowMapResolution, mShadowMapResolution);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Draw each object in the scene
        for (auto object : objectsWithShadow)
        {
            // Set the model matrix of the object in the shader
            mShadowShader->setMat4("model", object->getModelMatrix());
            // Draw the object
            object->draw();
        }
    }

    // Method to pass the light to a shader
    void DirectionalLight::configureShader(const Shader& lightingShader, 
                                           Shader* shadowShader, 
                                           unsigned int& indexDirectional, 
                                           unsigned int& indexSpot, 
                                           unsigned int& indexPoint,
                                           unsigned int& indexShadow)
    {
        // Copy the pointer to the shader
        mShadowShader = shadowShader;

        // Pass the light properties to the shader
        // The shader must be bound before calling this method
        lightingShader.setVec3("dirLights[" + std::to_string(indexDirectional) + "].color", mColor);
        lightingShader.setVec3("dirLights[" + std::to_string(indexDirectional) + "].position", mPosition);
        lightingShader.setVec3("dirLights[" + std::to_string(indexDirectional) + "].direction", mDirection);

        lightingShader.setFloat("dirLights[" + std::to_string(indexDirectional) + "].intensity", mIntensity);
        lightingShader.setFloat("dirLights[" + std::to_string(indexDirectional) + "].kLinear", mAttenLinear);
        lightingShader.setFloat("dirLights[" + std::to_string(indexDirectional) + "].kQuadratic", mAttenQuadratic);

        // Increase the counter of the directiona lights
        indexDirectional++;
    }

    // Method to pass the lightSpaceMatrix to a shader
    void DirectionalLight::configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                           unsigned int& indexSpot, unsigned int& indexPoint,
                                           unsigned int& indexShadow) const
    {
        // Bind the shadowmap texture to the corresponding texture unit
        glActiveTexture(GL_TEXTURE0 + indexShadow);
        glBindTexture(GL_TEXTURE_2D_ARRAY, mShadowMapTexture);

        // Pass also the index of the texture unit for the shadowmap, and the 
        // light space matrix
        shader.setMat4("dirLights[" + std::to_string(indexDirectional) + "].lightSpaceMatrix", mLightSpaceMatrix);
        shader.setInt("dirLights[" + std::to_string(indexDirectional) + "].shadowMap", indexShadow);
        // shader.setInt("dirLights[" + std::to_string(indexDirectional) + "].nrCascadeLevels", mNrShadowCascadeLevels);
        shader.setInt("nrCascadeLevels", mNrShadowCascadeLevels);
        for (int i = 0; i < mNrShadowCascadeLevels + 1; ++i)
        {
            shader.setFloat("dirLights[" + std::to_string(indexDirectional) + "].cascadeDistances["
                             + std::to_string(i) + "]", mShadowCascadeDistances[i]);
        }

        // Increase the counter of the directiona lights
        indexDirectional++;
        // Increase the counter of the shadow maps
        // indexShadow++;
        indexShadow += mNrShadowCascadeLevels;
        // indexShadow += 10;
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
        Light( color, position, intensity, attenLinear, attenQuadratic, shadowRes,
               LIGHT_SPOT )
    {
        // Outer angle
        mAngleOuter = glm::radians(angleOuter);
        // Compute the cosines of the two angles
        mCosAngleInner = glm::cos(glm::radians(angleInner / 2.f));
        mCosAngleOuter = glm::cos(glm::radians(angleOuter / 2.f));

        // Compute the maximum distance of influence of the light, where the intensity
        // becomes 5/256 times its value at d = 0
        float disc { mAttenLinear * mAttenLinear - 4.f * mAttenQuadratic * ( 1.f - mIntensity * 256.f / 5.f ) };
        mRadiusMax = ( - mAttenLinear + glm::sqrt(disc) ) / (2.f * mAttenQuadratic);

        // Compute the up direction
        // Create a temporary vector, to compute the up direction as being perpendicular
        // to this and the direction vector
        glm::vec3 tempVector { 0., 1., 0. };
        // Check if this vector is parallel to the direction, and modify it in
        // that case
        if (fabs( glm::length( glm::cross(mDirection, tempVector) ) ) <= 0.001)
        {
            tempVector = glm::vec3( 1., 0., 0. );
        }
        // Compute the up direction
        mUpDirection = glm::cross(tempVector, mDirection);

        // Configure the FBO and the texture for the shadowmap
        setupShadowMap();
    }

    // Method to configure the shadow map framebuffer and texture
    void SpotLight::setupShadowMap()
    {
         // Generate the framebuffer and bind it
        glGenFramebuffers(1, &mShadowMapFBO);

        // Generate the texture
        glGenTextures(1, &mShadowMapTexture);
        glBindTexture(GL_TEXTURE_2D, mShadowMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
             mShadowMapResolution, mShadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  

        // Use the texture as the depth attachment of the FBO
        glBindFramebuffer(GL_FRAMEBUFFER, mShadowMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        // Check if the framebuffer is complete
        int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            throw 0;
        }
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);  
    }

    // Method to compute the light space matrix
    // void computeLightSpaceMatrix(const Camera& camera);
    void SpotLight::computeLightSpaceMatrix()
    {
        // Light view matrix
        const glm::mat4 lightView { glm::lookAt( mPosition,              // Position of the light
                                                 mPosition + mDirection, // Center of the frustum 
                                                 mUpDirection ) };       // Up vector

    // std::cout << glm::to_string(mUpDirection) << std::endl;

        // Projection matrix for the light
        // This is simply a perspective projection, with the opening angle of the 
        // spot light and an aspect ratio of 1
        const glm::mat4 lightProjection { glm::perspective(mAngleOuter, 1.f, 0.1f, mRadiusMax) };

        // Compute the lightSpaceMatrix
        mLightSpaceMatrix = lightProjection * lightView;
    }

    // Method to compute the shadow map
    void SpotLight::computeShadowMap(const Camera& camera,
                                     const std::vector<GLElemObject*> objectsWithShadow)
    {
        // Compute the light space matrix
        computeLightSpaceMatrix();

        // Change the size of the viewport
        glViewport(0, 0, mShadowMapResolution, mShadowMapResolution);

        // Bind the FBO, whose depth attachment is the shadow map texture
        glBindFramebuffer(GL_FRAMEBUFFER, mShadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Pass the lightSpaceMatrix to the shader (which was already bound before
        // this function call)
        mShadowShader->use();
        mShadowShader->setMat4("lightSpaceMatrix", mLightSpaceMatrix);
        // Pass the position of the light and the far plane of its frustum to the shader
        mShadowShader->setFloat("farPlane", mRadiusMax);
        mShadowShader->setVec3("lightPos", mPosition);

        // // Set face culling to the front faces
        // glCullFace(GL_FRONT);
        // Draw each object in the scene
        for (auto object : objectsWithShadow)
        {
            // Set the model matrix of the object in the shader
            mShadowShader->setMat4("model", object->getModelMatrix());
            // Draw the object
            object->draw();
        }
        // // Restore face culling
        // glCullFace(GL_BACK);
    }

    // Method to pass the light to a shader
    void SpotLight::configureShader(const Shader& lightingShader, 
                                    Shader* shadowShader, 
                                    unsigned int& indexDirectional, 
                                    unsigned int& indexSpot, 
                                    unsigned int& indexPoint,
                                    unsigned int& indexShadow)
    {
        // Copy the pointer to the shader
        mShadowShader = shadowShader;

        // Pass the light properties to the shader
        // The shader must be bound before calling this method
        lightingShader.setVec3("spotLights[" + std::to_string(indexSpot) + "].color", mColor);
        lightingShader.setVec3("spotLights[" + std::to_string(indexSpot) + "].position", mPosition);
        lightingShader.setVec3("spotLights[" + std::to_string(indexSpot) + "].direction", mDirection);
        lightingShader.setVec3("spotLights[" + std::to_string(indexSpot) + "].upDirection", mUpDirection);

        lightingShader.setFloat("spotLights[" + std::to_string(indexSpot) + "].intensity", mIntensity);
        lightingShader.setFloat("spotLights[" + std::to_string(indexSpot) + "].kLinear", mAttenLinear);
        lightingShader.setFloat("spotLights[" + std::to_string(indexSpot) + "].kQuadratic", mAttenQuadratic);

        lightingShader.setFloat("spotLights[" + std::to_string(indexSpot) + "].cosAngleInner", mCosAngleInner);
        lightingShader.setFloat("spotLights[" + std::to_string(indexSpot) + "].cosAngleOuter", mCosAngleOuter);
        lightingShader.setFloat("spotLights[" + std::to_string(indexSpot) + "].radiusMax", mRadiusMax);

        // Increase the counter of the spot lights
        indexSpot++;
    }

    // Method to pass the lightSpaceMatrix to a shader
    void SpotLight::configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                           unsigned int& indexSpot, unsigned int& indexPoint,
                                           unsigned int& indexShadow) const
    {
        // Bind the shadowmap texture to the corresponding texture unit
        glActiveTexture(GL_TEXTURE0 + indexShadow);
        glBindTexture(GL_TEXTURE_2D, mShadowMapTexture);

        // Pass also the index of the texture unit for the shadowmap, and the 
        // light space matrix
        shader.setMat4("spotLights[" + std::to_string(indexSpot) + "].lightSpaceMatrix", mLightSpaceMatrix);
        // shader.setInt("spotLights[" + std::to_string(indexSpot) + "].shadowMap", indexShadow);
        shader.setInt("spotShadowMap", indexShadow);

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
        Light( color, position, intensity, attenLinear, attenQuadratic, shadowRes,
               LIGHT_POINT )
    {
        // Compute the maximum distance of influence of the light, where the intensity
        // becomes 5/256 times its value at d = 0
        float disc { mAttenLinear * mAttenLinear - 4.f * mAttenQuadratic * ( 1.f - mIntensity * 256.f / 5.f ) };
        mRadiusMax = ( - mAttenLinear + glm::sqrt(disc) ) / (2.f * mAttenQuadratic);
        
    }

    // Method to configure the shadow map framebuffer and texture
    void PointLight::setupShadowMap()
    {
        
    }

    // Method to compute the shadow map
    void PointLight::computeShadowMap(const Camera& camera,
                                      const std::vector<GLElemObject*> objectsWithShadow)
    {

        // // Bind the FBO, whose depth attachment is the shadow map texture
        // glBindFramebuffer(GL_FRAMEBUFFER, mShadowMapFBO);
        // glClear(GL_DEPTH_BUFFER_BIT);
        //
        // // Use the shader
        // mShadowShader->use();

    }

    // Method to pass the light to a shader
    void PointLight::configureShader(const Shader& lightingShader, 
                                     Shader* shadowShader, 
                                     unsigned int& indexDirectional, 
                                     unsigned int& indexSpot, 
                                     unsigned int& indexPoint,
                                     unsigned int& indexShadow)
    {
        // // Copy the pointer to the shader
        // mShadowShader = shadowShader;

        // Pass the light properties to the shader
        // The shader must be bound before calling this method
        lightingShader.setVec3("pointLights[" + std::to_string(indexPoint) + "].color", mColor);
        lightingShader.setVec3("pointLights[" + std::to_string(indexPoint) + "].position", mPosition);

        lightingShader.setFloat("pointLights[" + std::to_string(indexPoint) + "].intensity", mIntensity);
        lightingShader.setFloat("pointLights[" + std::to_string(indexPoint) + "].kLinear", mAttenLinear);
        lightingShader.setFloat("pointLights[" + std::to_string(indexPoint) + "].kQuadratic", mAttenQuadratic);
        lightingShader.setFloat("pointLights[" + std::to_string(indexPoint) + "].radiusMax", mRadiusMax);

        // Increase the count of the point lights
        indexPoint++;
    }

    // Method to pass the lightSpaceMatrix to a shader
    void PointLight::configureShaderForLightingPass(const Shader& shader, unsigned int& indexDirectional, 
                                           unsigned int& indexSpot, unsigned int& indexPoint,
                                           unsigned int& indexShadow) const
    {
        // // Bind the shadowmap texture to the corresponding texture unit
        // glActiveTexture(GL_TEXTURE0 + indexShadow);
        // glBindTexture(GL_TEXTURE_2D, mShadowMapTexture);

        // Pass also the index of the texture unit for the shadowmap, and the 
        // light space matrix
        shader.setMat4("pointLights[" + std::to_string(indexPoint) + "].lightSpaceMatrix", mLightSpaceMatrix);
        shader.setInt("pointLights[" + std::to_string(indexPoint) + "].shadowMap", indexShadow++);

        // Increase the count of the point lights
        indexPoint++;
        // // Increase the counter of the shadow maps
        // indexShadow++;
    }
}
