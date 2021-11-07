#ifndef GLOBJECT_H
#define GLOBJECT_H

#include "GLGeometry.h"

namespace GLGeometry
{
    class GLObject
    {
        protected:
            // Objects needed for rendering
            unsigned int mVAO;
            unsigned int mVBO;
            unsigned int mEBO;

            // Model matrix
            glm::mat4 mModelMatrix;

            // Functions to set the VAO, VBO and EBO from a child class
            // These are pure virtual functions, since their implementation will
            // be written directly in the child classes.
            virtual void setVAO() = 0;
            virtual void setVBO() = 0;
            virtual void setEBO() = 0;

        public:
            // Constructor
            GLObject() : mVAO { 0 }, mVBO { 0 }, mEBO { 0 }
            {
                // Create the Vertex array object
                glGenVertexArrays(1, &mVAO);
                // Create the Vertex buffer object
                glGenBuffers(1, &mVBO);
                // Create the Element buffer object
                glGenBuffers(1, &mEBO);
            }

            // Function to set the model matrix
            void setModelMatrix(const glm::vec3& translation, const float& rotationAngle, 
                                const glm::vec3& rotationAxis, const glm::vec3& scale)
            {
                mModelMatrix = glm::scale(glm::mat4(1.f), scale);
                mModelMatrix = glm::translate(mModelMatrix, translation);
                mModelMatrix = glm::rotate(mModelMatrix, rotationAngle, rotationAxis);
            }

            // Function to read the model matrix
            glm::mat4 getModelMatrix()
            {
                return mModelMatrix;
            }

            // Function to render
            virtual void draw() = 0;
    };
}

#endif
