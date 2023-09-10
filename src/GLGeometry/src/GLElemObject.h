#ifndef GLOBJECTELEMENTARY_H
#define GLOBJECTELEMENTARY_H

#include "GLGeometry.h"
#include "GLObject.h"
#include "utils.h"

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

            // Data of the mesh
            std::vector<GLBase::Vertex> mVertices;

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
            void setModelMatrix(const glm::vec3& translation, 
                                const glm::mat4& rotationMatrix, 
                                const glm::vec3& scale)
            {
                mModelMatrix = glm::mat4(1.f);
                mModelMatrix = glm::translate(mModelMatrix, translation);
                mModelMatrix = mModelMatrix * rotationMatrix;
                mModelMatrix = glm::scale(mModelMatrix, scale);
            }

            // Function to read the model matrix
            glm::mat4 getModelMatrix()
            {
                return mModelMatrix;
            }

            // Function to get the positions of the vertices
            std::vector<glm::vec3> getVertices()
            {
                // Initialize the vector
                std::vector<glm::vec3> vertices( mVertices.size() );

                // Store all the positions of the vertices
                for ( int i = 0; i < mVertices.size(); ++i )
                    vertices[ i ] = mVertices[ i ].Position;

                return vertices;
            }

            // // Function to render
            // virtual void draw() = 0;
    };

    class GLObjectPlaceholder : public GLElemObject
    {
        private:
            // // This class will use an element buffer object
            // unsigned int mEBO;

            // // Data of the mesh
            // // std::vector<Vertex> mVertices;
            // std::vector<unsigned int> mIndices;
            //
            // // Number of vertices in each circle
            // int mNrVertices;

        public:
            // Constructor
            GLObjectPlaceholder()
            {}

            // Function to render
            void draw()
            {}
    };
}

#endif
