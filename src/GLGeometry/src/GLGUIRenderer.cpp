#include "GLGUIRenderer.h"

/*
    This implementation is based on the tutorial
    https://learnopengl.com/In-Practice/GUI-Rendering
*/

using namespace GLBase;

namespace GLGeometry
{
    //==============================
    // Constructor
    //==============================

    // Constructor
    GLGUIRenderer::GLGUIRenderer( const int scrWidth, const int scrHeight ) :
        mWidth { scrWidth }, mHeight { scrHeight },
        mGUIShader( std::string(BASE_DIR_SHADERS) + "/GLGeometry/GUIVertex.glsl",
                     std::string(BASE_DIR_SHADERS) + "/GLGeometry/GUIFragment.glsl" )
        
    {
        // Generate the texture for the GUI
        generateGUITexture();

        // Create a projection matrix from the width and height of the screen
        mProjectionMatrix = glm::ortho( 0.0f, (float)scrWidth, 0.0f, (float)scrHeight );
        // Set this on the shader
        mGUIShader.use();
        mGUIShader.setMat4( "projection", mProjectionMatrix );

        // Set the index of the texture that will be used for the glyphs
        mGUIShader.setInt( "GUITexture", 0 );

        // Setup a VBO and VAO for rendering the quads
        glGenVertexArrays( 1, &mQuadVAO );
        glGenBuffers( 1, &mQuadVBO );
        // Bind the buffers
        glBindVertexArray( mQuadVAO );
        glBindBuffer( GL_ARRAY_BUFFER, mQuadVBO );
        // The 2D quad requires 6 vertices of 4 floats each.
        // We'll update the content of the VBO's memory often, so we set it to dynamic
        glBufferData( GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW );
        // Set the attribute pointer
        glEnableVertexAttribArray( 0 );
        glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0 );
        // Unbind the buffers
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
    }

    // Destructor
    GLGUIRenderer::~GLGUIRenderer()
    {
        // Clear the FBO used to clear the texture
        glDeleteFramebuffers(1, &mFramebuffer);
    }

    // Method to generate a blank texture for the GUI
    void GLGUIRenderer::generateGUITexture()
    {
        // Generate a framebuffer, that will be used to clear the texture
        glGenFramebuffers(1, &mFramebuffer);

        glGenTextures( 1, &mGUITextureID );
        glBindTexture( GL_TEXTURE_2D, mGUITextureID );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                      mWidth, mHeight,
                      0, GL_RGBA, GL_UNSIGNED_BYTE,
                      nullptr );
                      // face->glyph->bitmap.buffer );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Vertices of the quad in the screen
        float vertices[6][4] = {
            // Position             // Texture coordinates 
            { 0.f,     0.f + (float)mHeight,   0.0f, 0.0f },            
            { 0.f,     0.f,       0.0f, 1.0f },
            { 0.f + (float)mWidth, 0.f,       1.0f, 1.0f },

            { 0.f,     0.f + (float)mHeight,   0.0f, 0.0f },
            { 0.f + (float)mWidth, 0.f,       1.0f, 1.0f },
            { 0.f + (float)mWidth, 0.f + (float)mHeight,   1.0f, 0.0f }           
        };
        // Copy these to the corresponding member variable
        memcpy( mQuadVertices, vertices, 24*sizeof(float) );
    }

    // Method to clear the texture
    void GLGUIRenderer::clearGUI()
    {
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebuffer);
        // // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGUITextureID, 0);
        // glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mGUITextureID, 0); //Only need to do this once.
        // glDrawBuffer(GL_COLOR_ATTACHMENT0); //Only need to do this once.
        // GLuint clearColor[4] = {0, 0, 0, 0};
        // glClearBufferuiv(GL_COLOR, 0, clearColor);
        //
        //
        // // glBindTexture(GL_TEXTURE_2D, mGUITextureID);
        // // glClearTexImage(mGUITextureID, 0, GL_RGBA, GL_UNSIGNED_BYTE, {0, 0, 0, 0});
        // // // glClearTexImage(mGUITextureID, 0, GL_RGBA, GL_UNSIGNED_BYTE, &glm::u8vec4(255, 255, 255, 255)[0]);
		// // // glClearTexImage(TextureName[texture::COLORBUFFER], 0, GL_RGBA, GL_UNSIGNED_BYTE, &glm::u8vec4(255, 255, 255, 255)[0]);
        //
        // glBindTexture( GL_TEXTURE_2D, 0 );
    }

    // Method to draw an element to the GUI
    void GLGUIRenderer::pixelsToTexture( unsigned char pixels[], 
                                         const int width, const int height,
                                         const int posX, const int posY )
    {
        // Bind the texture you want to update
        glBindTexture(GL_TEXTURE_2D, mGUITextureID);

        // Update the texture with the new pixel data
        glTexSubImage2D(
            GL_TEXTURE_2D,     // Texture target
            0,                 // Mipmap level
            posX, posY,        // Offset in pixels
            width, height,     // Width and height of the region
            GL_RGBA,           // Format of the pixel data
            GL_UNSIGNED_BYTE,  // Data type of the pixel data
            pixels             // Pointer to the pixel data
        );

        // Unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Render a string of text
    void GLGUIRenderer::renderGUI()
    {
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Activate the shader
        mGUIShader.use();
        // Bind the texture and vertex array
        glActiveTexture( GL_TEXTURE0 );
        glBindVertexArray( mQuadVAO );

        // Update the VBO for the current character
        // Bind the glyph texture
        glBindTexture( GL_TEXTURE_2D, mGUITextureID );

        // Update the content of the VBO memory, to draw the quad
        glBindBuffer( GL_ARRAY_BUFFER, mQuadVBO );
        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(mQuadVertices), mQuadVertices );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        // Render the quad
        glDrawArrays( GL_TRIANGLES, 0, 6 );

        // Unbind the VAO and texture
        glBindVertexArray( 0 );
        glBindTexture( GL_TEXTURE_2D, 0 );

        // Enable depth test again
        glEnable(GL_DEPTH_TEST);
    }
}
