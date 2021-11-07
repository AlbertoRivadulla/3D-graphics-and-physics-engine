#ifndef GLQUAD_H
#define GLQUAD_H

#include "GLGeometry.h"
#include "GLObject.h"

using namespace GLBase;

namespace GLGeometry
{
    class GLQuad : public GLObject
    {
        private:
            // Functions to set the VAO, VBO and EBO from a child class
            void setVAO();
            void setVBO();
            void setEBO();

            // Data of the mesh
            std::vector<Vertex> mVertices;
            std::vector<unsigned int> mIndices;

        public:
            // Constructor
            GLQuad();

            // Function to render
            void draw();
    };
}



#endif
