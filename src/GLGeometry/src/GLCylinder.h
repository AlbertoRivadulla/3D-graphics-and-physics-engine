#ifndef GLCYLINDER_H
#define GLCYLINDER_H

#include "GLGeometry.h"
#include "GLElemObject.h"

using namespace GLBase;

namespace GLGeometry
{
    class GLCylinder : public GLElemObject
    {
        private:
            // This class will use an element buffer object
            unsigned int mEBO;

            // Data of the mesh
            // std::vector<Vertex> mVertices;
            std::vector<unsigned int> mIndices;

            // Number of vertices in each circle
            int mNrVertices;

        public:
            // Constructor
            GLCylinder(int nrVerticesCircle);

            // Function to render
            void draw();
    };
}

#endif
