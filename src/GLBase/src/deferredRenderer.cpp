#include "GLBase.h"
#include "shader.h"

namespace GLBase
{
    // Constructor
    // The scaling variable is the relation between the rendering resolution
    // and teh viewport resolution
    // Do this by defining a lower resolution framebuffer, and then blitting
    // its contents to the main one
    // https://community.khronos.org/t/creating-low-resolution-output-using-glblitframebuffer/75682/2
    DeferredRenderer::DeferredRenderer(int width, int height, float scaling) :
        mWinWidth { width }, mWinHeight { height }, 
        mRenderWidth { (int)(width / scaling) }, mRenderHeight { (int)(height / scaling) },
        mScreenShader("../shaders/GLBase/defRenderQuadVertex.glsl", 
                      "../shaders/GLBase/defRenderQuadFragment.glsl"),
        // mLightingPassShader("../shaders/GLBase/defGeometryPassVertex.glsl", 
        //              "../shaders/GLBase/defGeometryPassFragment.glsl")
        mLightingPassShader("../shaders/GLBase/defLightingPassVertex.glsl", 
                            "../shaders/GLBase/defLightingPassFragment.glsl")
    {
        // Color to clear the window
        glClearColor(1.f, 0.f, 1.f, 1.0f);

        // Setup the FBOs
        setupGBuffer();
        setupTargetBuffer();

        // Setup the screen quad
        setupScreenQuad();
    }

    // Destructor
    DeferredRenderer::~DeferredRenderer()
    {
        // Clear the FBOs
        glDeleteFramebuffers(1, &mTargetBuffer);
        glDeleteFramebuffers(1, &mGBuffer);
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
        mScreenShader.use();
        mScreenShader.setInt("screenTexture", 0);
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
        // It needs only 3 components per pixel, but I sue RGBA for hardware reasons
        glGenTextures(1, &mGPositionTexture);
        glBindTexture(GL_TEXTURE_2D, mGPositionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mRenderWidth, mRenderHeight, 0, 
                     GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
                               mGPositionTexture, 0);
        // 2 - Normal texture
        glGenTextures(1, &mGNormalTexture);
        glBindTexture(GL_TEXTURE_2D, mGNormalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mRenderWidth, mRenderHeight, 0, 
                     GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 
                               mGNormalTexture, 0);
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
        // This is not needed when doing deferred shading, since that framebuffer
        // will have its own depth and stencil attachments.
        glGenRenderbuffers(1, &mDepthRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRBO);
        // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mRenderWidth, mRenderHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
        // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRBO); // now actually attach it
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mRenderWidth, mRenderHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRBO);
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Configure the textures in the shader for the lighting pass
        mLightingPassShader.use();
        mLightingPassShader.setInt("gPosition", 0);
        mLightingPassShader.setInt("gNormal", 1);
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mRenderWidth, mRenderHeight, 0, 
                     GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        // Set the mipmap filtering of the texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Attach the texture to the FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTargetTexture, 0);

        // Tell opengl that which color attachment of this framebuffer we will use
        // for rendering
        unsigned int attachments[1] { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, attachments);

        // Configure the depth renderbuffer object, created in setupGBuffer()
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mRenderWidth, mRenderHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRBO);
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Method to call at the beginning of the frame
    void DeferredRenderer::startFrame()
    {
        // Change the viewport to the lower resolution
        glViewport(0, 0, mRenderWidth, mRenderHeight);

        // Clear the default buffer
        // I do this here instead of in endFrame() to have an accurate reading 
        // of the frametime
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind the g-buffer and clear it
        glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Method to do the shading pass with the information in the g-buffer
    void DeferredRenderer::processGBuffer( glm::vec3 viewPos )
    {
        // Bind the lower resolution FBO, and clear it 
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mTargetBuffer);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, mTargetBuffer);
        glClear(GL_COLOR_BUFFER_BIT);

        // Disable depth testing to draw the screen quad
        glDisable(GL_DEPTH_TEST);
        // Bind the texture attachments of the G-buffer, and setup the shader
        mLightingPassShader.use();
        mLightingPassShader.setVec3("viewPos", viewPos);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mGPositionTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mGNormalTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, mGAlbedoSpecTexture);
        // Draw the screen quad, performing the lighting calculations
        glBindVertexArray(mScreenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Enable depth testing again
        glEnable(GL_DEPTH_TEST);

        // // Copy the content of the GBuffer's depth component to the target framebuffer's
        // // depth component
        // glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer);
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mTargetBuffer); // write to default framebuffer
        // // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
        // // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
        // // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
        // glBlitFramebuffer(0, 0, mRenderWidth, mRenderHeight, 0, 0, mRenderWidth, mRenderHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        // glBindFramebuffer(GL_FRAMEBUFFER, mTargetBuffer);
    }

    // Method to call at the end of the frame
    void DeferredRenderer::endFrame()
    {
        // // Change the viewport to the full resolution
        // glViewport(0, 0, mWinWidth, mWinHeight);
        // // Bind the default framebuffer
        // // It was already cleared in startFrame()
        // glBindFramebuffer(GL_READ_FRAMEBUFFER, mTargetBuffer);
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        // // Disable depth testing to draw the screen quad
        // glDisable(GL_DEPTH_TEST);
        // // Bind the texture attachment of the render FBO, and setup the shader
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, mTargetTexture);
        // // Draw the screen quad
        // mScreenShader.use();
        // glBindVertexArray(mScreenVAO);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // // Enable depth testing again
        // glEnable(GL_DEPTH_TEST);

        // Instead of drawing the screen quad, I could do the following:
        // Bind the default framebuffer
        // glBindFramebuffer(GL_FRAMEBUFFER, mTargetBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        // Copy the result of the target FBO to the screen
        glBlitFramebuffer( 0, 0, mRenderWidth, mRenderHeight,
                           0, 0, mWinWidth,    mWinHeight,
                           GL_COLOR_BUFFER_BIT, GL_NEAREST );
    }
}
