#ifndef GLCONE_H
#define GLCONE_H

#include "GLGeometry.h"
#include "GLElemObject.h"

using namespace GLBase;

namespace GLGeometry
{
    class GLCone : public GLElemObject
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
            GLCone(int nrVerticesCircle);

            // Function to render
            void draw();
    };
}

#endif
