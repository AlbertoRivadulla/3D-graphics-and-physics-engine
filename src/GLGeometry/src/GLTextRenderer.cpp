#include "GLTextRenderer.h"

/*
    This implementation is based on the tutorial
    https://learnopengl.com/In-Practice/Text-Rendering
*/

using namespace GLBase;

namespace GLGeometry
{
    //==============================
    // Constructor
    //==============================

    // Constructor
    GLTextRenderer::GLTextRenderer( const int scrWidth, const int scrHeight, 
                                    const char* fontPath, int height ) : 
        mTextShader( "../shaders/GLGeometry/textVertex.glsl",
                     "../shaders/GLGeometry/textFragment.glsl" )
    {
        // Initialize freetype
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

        // Load the font
        FT_Face face;
        if (FT_New_Face(ft, fontPath, 0, &face))
            std::cout << "ERROR::FREETYPE: Failed to load font at " << fontPath << std::endl;  

        // Setup the pixel font size
        // The second argument is the width, and the third one the height.
        // If one is set to 0, it will be determined from the other one.
        FT_Set_Pixel_Sizes( face, 0, height );

        // Load the characters
        loadCharacters( face );

        // Clear the resources
        FT_Done_Face( face );
        FT_Done_FreeType( ft );

        // Create a projection matrix from the width and height of the screen
        mProjectionMatrix = glm::ortho( 0.0f, (float)scrWidth, 0.0f, (float)scrHeight );
        // Set this on the shader
        mTextShader.use();
        mTextShader.setMat4( "projection", mProjectionMatrix );

        // Set the index of the texture that will be used for the glyphs
        mTextShader.setInt( "glyphTexture", 0 );

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


    // Method to load the characters into the map
    void GLTextRenderer::loadCharacters( FT_Face& face )
    {
        // Disable byte alignment restriction.
        // This is needed because these textures in general will not have a 
        // 4-byte alignment between pixels, as they are not RGBA but only a 
        // grayscale value.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Load the first 128 ASCII characters
        for ( unsigned int ch = 0; ch < 128; ++ch )
        {
            // Load the character glyph
            if ( FT_Load_Char( face, ch, FT_LOAD_RENDER ) )
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph " << ch << std::endl;
                continue;
            }

            // Generate the texture
            unsigned int texture;
            glGenTextures( 1, &texture );
            glBindTexture( GL_TEXTURE_2D, texture );
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RED,
                          face->glyph->bitmap.width, 
                          face->glyph->bitmap.rows,
                          0, GL_RED, GL_UNSIGNED_BYTE,
                          face->glyph->bitmap.buffer );
            // Set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Store the character in the map
            Character character = {
                texture,
                glm::ivec2( face->glyph->bitmap.width, face->glyph->bitmap.rows ),
                glm::ivec2( face->glyph->bitmap_left, face->glyph->bitmap_top ),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            mCharsMap.insert( std::pair<char, Character>( ch, character ) );
        }
    }

    // Render a string of text
    void GLTextRenderer::renderText( const std::string& text, float x, const float y,
                                     const float scale, glm::vec3 color )
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Activate the shader
        mTextShader.use();
        // Bind the texture and vertex array
        glActiveTexture( GL_TEXTURE0 );
        glBindVertexArray( mQuadVAO );
        // Set the value of the text color
        mTextShader.setVec3( "textColor", color );

        // Iterate through the characters in the string
        std::string::const_iterator c;
        for ( c = text.begin(); c != text.end(); ++c )
        {
            // Retrieve the character from the map
            Character ch = mCharsMap[ *c ];

            // Compute its position
            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - ( ch.Size.y - ch.Bearing.y ) * scale;
            // Compute its dimensions 
            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            // Update the VBO for the current character
            float vertices[6][4] = {
                // Position             // Texture coordinates 
                { xpos,     ypos + h,   0.0f, 0.0f },            
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }           
            };
            // Bind the glyph texture
            glBindTexture( GL_TEXTURE_2D, ch.TextureID );
            // Update the content of the VBO memory
            glBindBuffer( GL_ARRAY_BUFFER, mQuadVBO );
            glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            // Render the quad
            glDrawArrays( GL_TRIANGLES, 0, 6 );

            // Advance the cursor to the next glyph
            x += ( ch.Advance >> 6 ) * scale;
        }

        // Unbind the VAO and texture
        glBindVertexArray( 0 );
        glBindTexture( GL_TEXTURE_2D, 0 );
    }
}
