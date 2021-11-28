#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include "GLBase.h"
#include "GLGeometry.h"

namespace GLBase
{
    class Light;

    class DeferredRenderer
    {
        public:
            // Constructor
            // The scaling variable is the relation between the rendering resolution
            // and teh viewport resolution
            DeferredRenderer(int width, int height, float scaling = 1.f);

            // Destructor
            ~DeferredRenderer();

            // Method to get a point to the geometry pass shader
            Shader& getLightingShader()
            {
                return mLightingPassShader;
            }

            // Method to configure the lights in the shader
            void configureLights(const std::vector<Light*> lights);
            // // Method to configure the light space matrices
            // // This needs to be called in each frame
            // void configureLightSpaceMatrices(const std::vector<Light*> lights);

            // Method to call at the beginning of the frame
            void startFrame();

            // Method to compute the shadow maps
            void computeShadowMaps(const Camera& camera, const std::vector<Light*> lightsWithShadow, 
                            const std::vector<GLGeometry::GLElemObject*> objectsWithShadow);

            // Method to call to start the geometry pass
            void startGeometryPass();

            // Configure the lights for the lighting pass
            void configureLightsForLightingPass(const std::vector<Light*> lights);

            // Method to do the shading pass with the information in the g-buffer
            void processGBuffer(glm::vec3 viewPos, const std::vector<Light*> lights);

            // Method to call at the end of the frame
            void endFrame(GLGeometry::GLCubemap* skyMap);
            // void endFrame(GLGeometry::GLCubemap* skyMap, GLGeometry::GLAuxElements auxElements);

        private:

            // Size of the window
            int mWinWidth;
            int mWinHeight;
            // Size of the render target
            int mRenderWidth;
            int mRenderHeight;

            // Renderbuffer object for the depth information, to be used by all 
            // passes
            unsigned int mDepthRBO;

            // Data for the shadow pass
            // ------------------------------
            // Shaders for computing the shadow maps
            Shader mShadowMapDirectionalShader;
            Shader mShadowMapPointShader;
            Shader mShadowMapSpotShader;

            // Data for the geometry pass
            // ------------------------------
            // G-buffer
            unsigned int mGBuffer;
            // Texture attachments for the G-buffer
            unsigned int mGPositionTexture;
            unsigned int mGNormalTexture;
            unsigned int mGAlbedoSpecTexture;

            // Data for the lighting pass
            // ------------------------------
            // Shader for the lighting pass
            Shader mLightingPassShader;

            // Data for rendering the screen quad
            // ------------------------------
            // Shader for rendering the target
            Shader mScreenShader;
            // Target framebuffer object
            unsigned int mTargetBuffer;
            // Texture attachment for the target FBO
            unsigned int mTargetTexture;
            // VAO and VBO for the screen quad
            unsigned int mScreenVAO;
            unsigned int mScreenVBO;

            // Setup the screen quad
            void setupScreenQuad();

            // Setup the target FBO
            void setupTargetBuffer();

            // Setup the G-buffer
            void setupGBuffer();
    };
}

#endif
