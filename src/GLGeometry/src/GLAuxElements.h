#ifndef GLAUXELEMENTS_H
#define GLAUXELEMENTS_H


#include "GLGeometry.h"
#include "GLElemObject.h"

using namespace GLBase;

namespace GLGeometry
{
    // This class will be used to draw:
    //  - Points
    //  - Lines
    //  - Rectangles
    //  - Boxes
    //  - Spheres
    class GLAuxElements
    {
        public:
            // Constructor
            GLAuxElements(int width, int height);

            // Method to change the dimensions of the screen
            void setViewportSize(int width, int height);

            // Method to change the color of the point
            void setPointColor(glm::vec3 color);
            // Method to set the size of the point
            void setPointSize(int size);
            // Method to draw a point
            void drawPoint(const glm::vec3& position, const glm::mat4& view, 
                           const glm::mat4& projection);

            // Method to change the color of the line
            void setLineColor(glm::vec3 color);

            // Method to draw a line
            void drawLine(const glm::vec3& p1, const glm::vec3& p2, 
                          const glm::mat4& view, const glm::mat4& projection);

            // Method to draw a rectangle
            void drawRectangle(const glm::vec3& translation, const float& rotationAngle, 
                               const glm::vec3& rotationAxis, const glm::vec3& scale,
                               const glm::mat4& view, const glm::mat4& projection);

            // Method to draw a parallelepiped with a vertex at the origin, its base
            // along the vector vec and its two dimensions as given
            void drawParallelepiped(const glm::vec3& origin, const glm::vec3& v1, const glm::vec3& v2, 
                               const glm::mat4& view, glm::mat4 projection);

            // Method to draw a box
            void drawBox(const glm::vec3& translation, const float& rotationAngle, 
                               const glm::vec3& rotationAxis, const glm::vec3& scale,
                               const glm::mat4& view, const glm::mat4& projection);

            // Method to draw a cylinder
            void drawCylinder(const glm::vec3& translation, const float& rotationAngle, 
                               const glm::vec3& rotationAxis, const glm::vec3& scale,
                               const glm::mat4& view, const glm::mat4& projection);

        private:
            // Objects needed for rendering points
            unsigned int mPointVAO;
            unsigned int mPointVBO;
            Shader mPointShader;
            // Method to set the VAO, VBO and properties of the point
            void setupPoint();

            // Objects needed for rendering lines
            unsigned int mLineVAO;
            unsigned int mLineVBO;
            Shader mLineShader;
            // Method to set the VAO, VBO and properties of the Line
            void setupLine();

            // Objects needed for rendering rectangles
            unsigned int mRectangleVAO;
            unsigned int mRectangleVBO;
            unsigned int mRectangleEBO;
            // Method to set the VAO, VBO and properties of the rectangle
            void setupRectangle();

            // Objects needed for rendering boxes
            unsigned int mBoxVAO;
            unsigned int mBoxVBO;
            unsigned int mBoxEBO;
            // Method to set the VAO, VBO and properties of the rectangle
            void setupBox();

            // Number of vertices in the bases of the cylinder
            int mNrVerticesCylinder;
            // Objects needed for rendering cylinder
            unsigned int mCylinderVAO;
            unsigned int mCylinderVBO;
            unsigned int mCylinderEBO;
            // Method to set the VAO, VBO and properties of the rectangle
            void setupCylinder();
    };
}

#endif




