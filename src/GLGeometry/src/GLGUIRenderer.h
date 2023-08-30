#ifndef GL_GUI_RENDERER_CPP
#define GL_GUI_RENDERER_CPP

#include "GLGeometry.h"
#include <map>

// Freetype library for rendering text
#include <ft2build.h>
#include FT_FREETYPE_H  

using namespace GLBase;

namespace GLGeometry
{
    // Class to draw a GUI on the screen
    class GLGUIRenderer
    {
        public:
            // Constructor
            GLGUIRenderer( const int scrWidth, const int scrHeight );

            // Destructor
            ~GLGUIRenderer();

            // Method to clear the texture
            void clearGUI();

            // Method to draw an element to the GUI
            void pixelsToTexture( unsigned char pixels[], 
                                  const int width, const int height,
                                  const int posX, const int posY );

            // Render the GUI
            void renderGUI();

        private:
            // Width and height of the screen
            int mWidth;
            int mHeight;
            // Vertices of the quad
            float mQuadVertices[24];
            // Shader
            Shader mGUIShader;
            // Texture
            unsigned int mGUITextureID;
            // Framebuffer for emptying the texture
            unsigned int mFramebuffer;
            // Projection matrix
            glm::mat4 mProjectionMatrix;
            // VBO and VAO for rendering the quads
            unsigned int mQuadVBO;
            unsigned int mQuadVAO;

            // Method to generate a blank texture for the GUI
            void generateGUITexture();
    };
}


# endif
