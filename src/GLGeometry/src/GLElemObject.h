#ifndef GLOBJECTELEMENTARY_H
#define GLOBJECTELEMENTARY_H

#include "GLGeometry.h"
#include "GLObject.h"

namespace GLGeometry
{
    class GLElemObject : public GLObject
    {
        protected:
            // Objects needed for rendering
            unsigned int mVAO;
            unsigned int mVBO;
            // Not all derived classes will have a EBO!
            // unsigned int mEBO;

            // Model matrix
            glm::mat4 mModelMatrix;

        public:
            // Constructor
            GLElemObject() : mVAO { 0 }, mVBO { 0 }// , mEBO { 0 }
            {
                // Create the Vertex array object
                glGenVertexArrays(1, &mVAO);
                // Create the Vertex buffer object
                glGenBuffers(1, &mVBO);
                // // Create the Element buffer object
                // glGenBuffers(1, &mEBO);
            }

            // Destructor
            virtual ~GLElemObject() = default;

            // Function to set the model matrix
            void setModelMatrix(const glm::vec3& translation, const float& rotationAngle, 
                                const glm::vec3& rotationAxis, const glm::vec3& scale)
            {
                mModelMatrix = glm::mat4(1.f);
                mModelMatrix = glm::translate(mModelMatrix, translation);
                if (rotationAngle != 0.)
                    mModelMatrix = glm::rotate(mModelMatrix, glm::radians(rotationAngle), 
                                               glm::normalize(rotationAxis));
                mModelMatrix = glm::scale(mModelMatrix, scale);
            }

            // Function to read the model matrix
            glm::mat4 getModelMatrix()
            {
                return mModelMatrix;
            }

            // // Function to render
            // virtual void draw() = 0;
    };
}

#endif
