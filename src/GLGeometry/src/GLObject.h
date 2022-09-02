#ifndef GLOBJECT_H
#define GLOBJECT_H

#include "GLGeometry.h"

namespace GLGeometry
{
    class GLObject
    {
        public:
            // Constructor
            GLObject() {};

            // Destructor
            virtual ~GLObject() = default;

            // Function to render
            virtual void draw() = 0;

            // Function to set the model matrix
            // virtual void setModelMatrix(const glm::vec3& translation, const float& rotationAngle, 
            //                             const glm::vec3& rotationAxis, const glm::vec3& scale) = 0;

            // Function to read the model matrix
            // virtual glm::mat4 getModelMatrix() = 0;
    };
}

#endif
