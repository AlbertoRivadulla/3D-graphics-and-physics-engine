#ifndef GLCUBE_H
#define GLCUBE_H

#include "GLGeometry.h"
#include "GLElemObject.h"

using namespace GLBase;

namespace GLGeometry
{
    class GLCube : public GLElemObject
    {
        private:
            // Data of the mesh
            // std::vector<Vertex> mVertices;
            std::vector<unsigned int> mIndices;

        public:
            // Constructor
            GLCube();

            // Function to render
            void draw();
    };
}

#endif
