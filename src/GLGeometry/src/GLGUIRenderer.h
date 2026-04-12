#ifndef GL_GUI_RENDERER_CPP
#define GL_GUI_RENDERER_CPP

#include "GLGeometry.h"
#include <map>

// Freetype library for rendering text
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace GLBase;

namespace GLGeometry {
// Class to draw a GUI on the screen
class GLGUIRenderer {
public:
    GLGUIRenderer(const int scrWidth, const int scrHeight);

    ~GLGUIRenderer();

    void setGUISize(int width, int height);

    std::pair<int, int> getGUISize();

    std::vector<unsigned char> &getPixels();

    // Method to clear the texture
    void clearGUI();

    // Method to draw an element to the GUI
    void pixelsToTexture(const int posX, const int posY);

    // Render the GUI
    void renderGUI();

private:
    // Pixels for the GUI
    int mGUIWidth;
    int mGUIHeight;

    std::vector<unsigned char> mGUIPixels;

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
} // namespace GLGeometry

#endif
