#ifndef GLQUAD_H
#define GLQUAD_H

#include "GLGeometry.h"
#include "GLElemObject.h"

using namespace GLBase;

namespace GLGeometry
{
    class GLQuad : public GLElemObject
    {
        private:
            // This class will use an element buffer object
            unsigned int mEBO;

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
