#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include "GLBase.h"

namespace GLBase
{
    class DeferredRenderer
    {
        public:
            // Constructor
            // The scaling variable is the relation between the rendering resolution
            // and teh viewport resolution
            DeferredRenderer(int width, int height, float scaling = 1.f);

            // Destructor
            ~DeferredRenderer();

            // Method to call at the beginning of the frame
            void startFrame();

            // Method to call at the end of the frame
            void endFrame();

        private:
            // Size of the window
            int mWinWidth;
            int mWinHeight;
            // Size of the render target
            int mRenderWidth;
            int mRenderHeight;

            // Target framebuffer object
            unsigned int mTargetBuffer;
            // Texture attachment for the target FBO
            unsigned int mTextureTarget;
            // Shader for rendering the target
            Shader mScreenShader;
            // VAO and VBO for the screen quad
            unsigned int mScreenVAO;
            unsigned int mScreenVBO;

            // G-buffer
            unsigned int mGBuffer;
            // Texture attachments for the G-buffer
            // ...
            // ...

            // Setup the screen quad
            void setupScreenQuad();

            // Setup the target FBO
            void setupTargetBuffer();

            // Setup the G-buffer
            void setupGBuffer();
    };
}

#endif
