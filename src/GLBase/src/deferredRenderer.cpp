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
        mScreenShader("../shaders/GLBase/renderQuadVertex.glsl", "../shaders/GLBase/renderQuadFragment.glsl")
    {
        // Color to clear the window
        glClearColor(0.f, 0.f, 0.f, 1.0f);

        // Setup the FBOs
        setupTargetBuffer();
        setupGBuffer();

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

    // Setup the target FBO
    void DeferredRenderer::setupTargetBuffer()
    {
        // Generate the target framebuffer
        glGenFramebuffers(1, &mTargetBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mTargetBuffer);

        // Generate the texture attachment for it
        glGenTextures(1, &mTextureTarget);
        glBindTexture(GL_TEXTURE_2D, mTextureTarget);
        // Configure the texture
        // The last 0 means that it is initially empty
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mRenderWidth, mRenderHeight, 0, 
                     GL_RGB, GL_UNSIGNED_BYTE, NULL);
        // Set the mipmap filtering of the texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Attach the texture to the FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureTarget, 0);

        // Tell opengl that which color attachment of this framebuffer we will use
        // for rendering
        unsigned int attachments[1] { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, attachments);

        // ---------------------------------------------------------------------

        // Create a renderbuffer object for the depth and stencil attachments of the
        // target framebuffer.
        // This is not needed when doing deferred shading, since that framebuffer
        // will have its own depth and stencil attachments.
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mRenderWidth, mRenderHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Setup the G-buffer
    void DeferredRenderer::setupGBuffer()
    {
        // Generate the g-buffer
        glGenBuffers(1, &mGBuffer);

        //...
        //...
        //...
    }

    // Method to call at the beginning of the frame
    void DeferredRenderer::startFrame()
    {
        // Change the viewport to the lower resolution
        glViewport(0, 0, mRenderWidth, mRenderHeight);

        // Here I would bind the g-buffer
        // Also clear it
        // ...
        // ...
        // ...

        // Clear the default buffer
        // I do this here to have an accurate reading of the frametime
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind the lower resolution FBO, and clear it 
        glBindFramebuffer(GL_FRAMEBUFFER, mTargetBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Method to call at the end of the frame
    void DeferredRenderer::endFrame()
    {
        // Here I would do the shading pass, writing to the target FBO
        // // Bind the lower resolution FBO
        // glBindFramebuffer(GL_FRAMEBUFFER, mTargetBuffer);
        // ...
        // ...
        // ...

        // Change the viewport to the full resolution
        glViewport(0, 0, mWinWidth, mWinHeight);
        // Bind the default framebuffer
        // It was already cleared in startFrame()
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        // Disable depth testing to draw the screen quad
        glDisable(GL_DEPTH_TEST);
        // Bind the texture attachment of the render FBO, and setup the shader
        glBindTexture(GL_TEXTURE_2D, mTextureTarget);
        // Draw the screen quad
        mScreenShader.use();
        glBindVertexArray(mScreenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Enable depth testing again
        glEnable(GL_DEPTH_TEST);

        // // Instead of drawing the screen quad, I could do the following:
        // // Bind the default framebuffer
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        // // Copy the result of the target FBO to the screen
        // glBlitFramebuffer( 0, 0, mRenderWidth, mRenderHeight,
        //                    0, 0, mWinWidth,    mWinHeight,
        //                    GL_COLOR_BUFFER_BIT, GL_NEAREST );
    }
}
