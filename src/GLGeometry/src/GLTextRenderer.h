#ifndef GL_TEXT_RENDERER_CPP
#define GL_TEXT_RENDERER_CPP

/*
    This implementation is based on the tutorial
    https://learnopengl.com/In-Practice/Text-Rendering
*/

#include "GLGeometry.h"
#include <map>

// Freetype library for rendering text
#include <ft2build.h>
#include FT_FREETYPE_H  

using namespace GLBase;

namespace GLGeometry
{
    // Struct for a single character's texture
    struct Character 
    {
        unsigned int TextureID;
        glm::ivec2   Size;
        glm::ivec2   Bearing;
        unsigned int Advance;
    };

    // Class to draw text on the screen
    class GLTextRenderer
    {
        public:
            // Constructor
            // GLTextRenderer( const int scrWidth, const int scrHeight, 
            //                 const char* fontPath, int height = 48 );
            GLTextRenderer( const int scrWidth, const int scrHeight, 
                            const std::string fontPath, int height = 48 );

            // Render a string of text
            void renderText( const std::string& text, float x, const float y,
                             const float scale, glm::vec3 color );

        private:
            // Map of characters
            std::map<char, Character> mCharsMap;
            // Shader
            Shader mTextShader;
            // Projection matrix
            glm::mat4 mProjectionMatrix;
            // VBO and VAO for rendering the quads
            unsigned int mQuadVBO;
            unsigned int mQuadVAO;

            // Method to load the characters into the map
            void loadCharacters( FT_Face& face );

    };
}

#endif
