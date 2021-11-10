#ifndef GLGIZMO_H
#define GLGIZMO_H

#include "GLGeometry.h"
#include "GLElemObject.h"

using namespace GLBase;

namespace GLGeometry
{
    class GLGizmo
    {
        public:
            // Constructor
            GLGizmo(glm::vec3 position, int radius, int widht, int height,
                    glm::vec3 color = glm::vec3(1., 1., 0.));

            // Method to set the color of the gizmo
            void setColor(glm::vec3 color);
            // Method to set the position of the gizmo
            void setPosition(glm::vec3 position);
            // Method to set the size of the gizmo
            void setSize(int size);

            // Draw method
            void draw(glm::mat4 view, glm::mat4 projection);

        private:
            // Model matrix
            glm::mat4 mModelMatrix;

            // Shader
            Shader mShader;

            unsigned int mVAO;
            unsigned int mVBO;
    };
}

#endif
