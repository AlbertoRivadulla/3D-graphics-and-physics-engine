#ifndef GLSPHERE_H
#define GLSPHERE_H

#include "GLGeometry.h"
#include "GLElemObject.h"

namespace GLGeometry
{
    class GLSphere : public GLElemObject
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
            GLSphere(int nrVertices);

            // Function to render
            void draw();
    };
}

#endif
