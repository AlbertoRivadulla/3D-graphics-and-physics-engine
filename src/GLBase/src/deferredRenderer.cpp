#include "GLBase.h"
#include "shader.h"

using namespace GLGeometry;

namespace GLBase
{
    // Constructor
    DeferredRenderer::DeferredRenderer() :
        mShadowMapDirectionalShader(std::string(BASE_DIR_SHADERS) + "/GLBase/shadowMapCascadedVertex.glsl", 
                                    std::string(BASE_DIR_SHADERS) + "/GLBase/shadowMapCascadedFragment.glsl",
                                    std::string(BASE_DIR_SHADERS) + "/GLBase/shadowMapCascadedGeometry.glsl"),
        mShadowMapPointShader(std::string(BASE_DIR_SHADERS) + "/GLBase/shadowMapVertex.glsl", 
                                    std::string(BASE_DIR_SHADERS) + "/GLBase/shadowMapFragment.glsl"),
        mShadowMapSpotShader(std::string(BASE_DIR_SHADERS) + "/GLBase/shadowMapSpotVertex.glsl", 
                                    std::string(BASE_DIR_SHADERS) + "/GLBase/shadowMapSpotFragment.glsl"),
        mLightingPassShader(std::string(BASE_DIR_SHADERS) + "/GLBase/defLightingPassVertex.glsl", 
                            std::string(BASE_DIR_SHADERS) + "/GLBase/defLightingPassFragment.glsl"),
        mScreenPostprocessingShader(std::string(BASE_DIR_SHADERS) + "/GLBase/defRenderQuadVertex.glsl", 
                      std::string(BASE_DIR_SHADERS) + "/GLBase/defRenderQuadPostproFragment.glsl")
    {
        // Color to clear the window
        glClearColor(1.f, 0.f, 1.f, 1.0f);
        glClearDepth(1.f);

        // Enable and configure stencil testing
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  
    }

    // Destructor
    DeferredRenderer::~DeferredRenderer()
    {
        // Clear the FBOs
        glDeleteFramebuffers(1, &mTargetBuffer);
        glDeleteFramebuffers(1, &mGBuffer);
    }

    // Setup the dimensions for the renderer
    // The scaling variable is the relation between the rendering resolution
    // and the viewport resolution
    // Do this by defining a lower resolution framebuffer, and then blitting
    // its contents to the main one
    // https://community.khronos.org/t/creating-low-resolution-output-using-glblitframebuffer/75682/2
    void DeferredRenderer::setupDimensions(int winWidth, int winHeight, int width, 
                                           int height, float scaling)
    {
        // Setup the diemnsions of the window
        mWinWidth = winWidth;
        mWinHeight = winHeight; 
        // Setup the dimensions of the renderer
        mRenderWidth = (int)(width / scaling); 
        mRenderHeight = (int)(height / scaling);

        // Setup the FBOs
        setupGBuffer();
        setupTargetBuffer();

        // Setup the screen quad
        setupScreenQuad();

        // Enable and configure stencil testing
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  
    }

    // Setup the screen quad
    void DeferredRenderer::setupScreenQuad()
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

        // Configure the screen texture in the shader
        mScreenPostprocessingShader.use();
        mScreenPostprocessingShader.setInt("screenTexture", 0);
        mScreenPostprocessingShader.setInt("gPosition", 1);
        mScreenPostprocessingShader.setVec3("fogColor", glm::vec3(0.43, 0.81, 0.92));
    }

    // Setup the G-buffer
    void DeferredRenderer::setupGBuffer()
    {
        // Generate the g-buffer
        glGenFramebuffers(1, &mGBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer);

        // Generate the texture attachments for the G-buffer
        // ------------------------------
        // 1 - Position texture
        // It needs only 3 components per pixel, but I use RGBA for hardware reasons
        glGenTextures(1, &mGPositionTexture);
        glBindTexture(GL_TEXTURE_2D, mGPositionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mRenderWidth, mRenderHeight, 0, 
                     GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
                               mGPositionTexture, 0);
        // 2 - Normal and emissive texture
        glGenTextures(1, &mGNormalEmissTexture);
        glBindTexture(GL_TEXTURE_2D, mGNormalEmissTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mRenderWidth, mRenderHeight, 0, 
                     GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 
                               mGNormalEmissTexture, 0);
        // 3 - Albedo and specular texture
        glGenTextures(1, &mGAlbedoSpecTexture);
        glBindTexture(GL_TEXTURE_2D, mGAlbedoSpecTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mRenderWidth, mRenderHeight, 0, 
                     GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, 
                               mGAlbedoSpecTexture, 0);

        // Configure these textures to be the color attachments of this Framebuffer
        unsigned int attachments[3] { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, 
                                      GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        // Create a renderbuffer object for the depth and stencil attachments of the
        // target framebuffer.
        // This same RBO will be shared with the target FBO, which won't write to
        // it during the lighting pass.
        glGenRenderbuffers(1, &mDepthRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mRenderWidth, mRenderHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRBO);

        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Configure the textures in the shader for the lighting pass
        mLightingPassShader.use();
        mLightingPassShader.setInt("gPosition", 0);
        mLightingPassShader.setInt("gNormalEmiss", 1);
        mLightingPassShader.setInt("gAlbedoSpec", 2);
    }

    // Setup the target FBO
    void DeferredRenderer::setupTargetBuffer()
    {
        // Generate the target framebuffer
        glGenFramebuffers(1, &mTargetBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mTargetBuffer);

        // Generate the texture attachment for it
        glGenTextures(1, &mTargetTexture);
        glBindTexture(GL_TEXTURE_2D, mTargetTexture);
        // Configure the texture
        // The last 0 means that it is initially empty
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mRenderWidth, mRenderHeight, 0, 
                     GL_RGBA, GL_FLOAT, NULL);
        // Set the mipmap filtering of the texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Attach the texture to the FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTargetTexture, 0);

        // Tell opengl that which color attachment of this framebuffer we will use
        // for rendering
        unsigned int attachments[1] { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, attachments);

        // Configure the depth and stencil renderbuffer object, created in setupGBuffer()
        // This is the same RBO for both FBOs
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRBO);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRBO);

        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Method to configure the lights in the shader
    void DeferredRenderer::configureLights(const std::vector<Light*> lights, glm::vec3 ambientLightColor)
    {
        // Counters for each type of light
        unsigned int countDirLights { 0 };
        unsigned int countSpotLights { 0 };
        unsigned int countPointLights { 0 };
        // Counter for the index of the shadow map
        // It starts in 3, because the three first ones correspond to the three textures
        // of the Geometry pass
        unsigned int countShadowMap { 3 };

        // Pass all the lights to the shader
        mLightingPassShader.use();
        for (auto light : lights)
        {
            // Check the type of the shader
            Shader* shadowShader;
            switch (light->getLightType())
            {
                case LIGHT_DIRECTIONAL:
                    shadowShader = &mShadowMapDirectionalShader;
                    break;
                case LIGHT_POINT:
                    shadowShader = &mShadowMapPointShader;
                    break;
                case LIGHT_SPOT:
                    shadowShader = &mShadowMapSpotShader;
                    break;
            }

            // This configures the shader for the lighting pass, and also passes
            // a pointer to the corresponding shader for the shadow pass, so it
            // is stored by the light object
            light->configureShader(mLightingPassShader, shadowShader, countDirLights, 
                                   countSpotLights, countPointLights, countShadowMap);
        }

        // Pass the count of each type of light to the shader
        mLightingPassShader.setInt("nrDirLights", countDirLights);
        mLightingPassShader.setInt("nrSpotLights", countSpotLights);
        mLightingPassShader.setInt("nrPointLights", countPointLights);

        // Set the ambient light color
        mLightingPassShader.setVec3("ambientLightColor", ambientLightColor);
    }

    // Method to call at the beginning of the frame
    void DeferredRenderer::startFrame()
    {
        // Clear the default buffer
        // I do this here instead of in endFrame() to have an accurate reading 
        // of the frametime
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Method to compute the shadow maps
    void DeferredRenderer::computeShadowMaps(const Camera& camera, 
                            const std::vector<Light*> lightsWithShadow, 
                            const std::vector<GLElemObject*> objectsWithShadow)
    {
        // Set face culling to the front faces
        // glCullFace(GL_FRONT);

        // Compute the shadow map for each light in the provided list
        for (auto light : lightsWithShadow)
        {
            light->computeShadowMap(camera, objectsWithShadow);
        }

        // Restore face culling
        // glCullFace(GL_BACK);
    }

    // Method to call to start the geometry pass
    void DeferredRenderer::startGeometryPass()
    {
        // Change the viewport to the lower resolution
        glViewport(0, 0, mRenderWidth, mRenderHeight);

        // Enable stencil testing
        // glEnable(GL_STENCIL_TEST);

        // Bind the g-buffer and clear it
        glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer);
        // Set the mask of the stencil buffer so the following writes to it.
        // This is needed also for clearing it!
        glStencilMask(0xFF); // Enable writing to the stencil buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Disable blending. This will be enabled later when computing the
        // contribution of each light separately
        glDisable(GL_BLEND);

        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // All fragments should pass the stencil test
    }

    // Configure the lights for the lighting pass
    void DeferredRenderer::configureLightsForLightingPass(const std::vector<Light*> lights)
    {
        // Counters for each type of light
        unsigned int countDirLights { 0 };
        unsigned int countSpotLights { 0 };
        unsigned int countPointLights { 0 };
        // Counter for the index of the shadow map
        // It starts in 3, because the three first ones correspond to the three textures
        // of the Geometry pass
        unsigned int countShadowMap { 3 };

        // Pass all the lights to the shader
        mLightingPassShader.use();
        for (auto light : lights)
        {
            light->configureShaderForLightingPass(mLightingPassShader, countDirLights, countSpotLights, 
                                   countPointLights, countShadowMap);
        }

        // Pass the count of each type of light to the shader
        mLightingPassShader.setInt("nrDirLights", countDirLights);
        mLightingPassShader.setInt("nrSpotLights", countSpotLights);
        mLightingPassShader.setInt("nrPointLights", countPointLights);
    }

    // Method to do the shading pass with the information in the g-buffer
    void DeferredRenderer::processGBuffer(glm::vec3 viewPos, const std::vector<Light*> lights)
    {
        // Bind the lower resolution FBO, and clear it 
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mTargetBuffer);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, mTargetBuffer);
        glClear(GL_COLOR_BUFFER_BIT);

        // // Enable additive blending, for drawing the differnt light contributions
        // glEnable(GL_BLEND);
        // glBlendEquation(GL_FUNC_ADD);
        // glBlendFunc(GL_ONE, GL_ONE);

        // Disable depth testing and enable stencil testing to draw the screen quad
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00); // Disable writing to the stencil buffer
        // Bind the texture attachments of the G-buffer, and setup the shader
        mLightingPassShader.use();
        mLightingPassShader.setVec3("viewPos", viewPos);
        // Bind the textures from the geometry pass
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mGPositionTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mGNormalEmissTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, mGAlbedoSpecTexture);
        // glBindTexture(GL_TEXTURE_2D, mDepthRBO);
        // Configure the lights
        configureLightsForLightingPass(lights);
        // Draw the screen quad, performing the lighting calculations
        glBindVertexArray(mScreenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Enable depth testing again
        glEnable(GL_DEPTH_TEST);
        // Disable stencil testing
        glDisable(GL_STENCIL_TEST);
    }

    // Method to call at the end of the frame
    void DeferredRenderer::endFrame(GLGeometry::GLCubemap* skyMap)
    {
        // glDepthMask(GL_FALSE);
        // Enable stencil testing for drawing the skymap
        glEnable(GL_STENCIL_TEST);
        // The skymap will be drawn where there is no geometry
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); // Disable writing to the stencil buffer
        // Draw the skymap
        skyMap->drawFlat();
        // skyMap->draw(); // If the skymap has textures
        // glDepthMask(GL_TRUE);

        // Change the viewport to the full resolution
        glViewport(0, 0, mWinWidth, mWinHeight);
        // Bind the default framebuffer
        // It was already cleared in startFrame()
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mTargetBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Disable depth testing to draw the screen quad
        glDisable(GL_DEPTH_TEST);
        // Bind the texture attachment of the render FBO, and setup the shader
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTargetTexture);
        // Bind the position texture from the G-buffer, to compute the fog contribution
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mGPositionTexture);
        // Draw the screen quad
        mScreenPostprocessingShader.use();
        glBindVertexArray(mScreenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Enable depth testing again
        glEnable(GL_DEPTH_TEST);
        // Disable stencil testing
        // glDisable(GL_STENCIL_TEST);

        // // Copy the depth information from the g-buffer to the default buffer
        // // before drawing the skymap
        // glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer);
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        // glBlitFramebuffer( 0, 0, mRenderWidth, mRenderHeight,
        //                    0, 0, mWinWidth,    mWinHeight,
        //                    GL_DEPTH_BUFFER_BIT, GL_NEAREST );
        // // Draw the skymap
        // skyMap->drawFlat();

        // // Instead of drawing the screen quad, I do the following:
        // // Bind the default framebuffer for drawing to it
        // glBindFramebuffer(GL_FRAMEBUFFER, mTargetBuffer);
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        // // Copy the result of the target FBO to the screen
        // // Recall that the target framebuffer is still bound, so it is used as the
        // // read framebuffer
        // glBlitFramebuffer( 0, 0, mRenderWidth, mRenderHeight,
        //                    0, 0, mWinWidth,    mWinHeight,
        //                    GL_COLOR_BUFFER_BIT, GL_NEAREST );
    }
}
