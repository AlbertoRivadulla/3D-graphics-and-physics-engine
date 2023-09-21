#ifndef GLTERRAINPATCH_H
#define GLTERRAINPATCH_H

#include "GLGeometry.h"
#include "GLElemObject.h"

namespace GLGeometry
{
    class GLTerrainPatch : public GLElemObject
    {
        protected:
            // This class will use an element buffer object
            unsigned int mEBO;

            // Data of the mesh
            // std::vector<Vertex> mVertices;
            std::vector<unsigned int> mIndices;

            // Number of strips and vertices per strip
            unsigned int mNumStrips;
            unsigned int mNumVertsPerStrip;

        public:
            // Constructor
            GLTerrainPatch();
            GLTerrainPatch( unsigned char* data, int width, int height, 
                            int nChannels );

            // Function to render
            virtual void draw();
    };


    class GLTerrainTessellated : public GLTerrainPatch
    {
        private:
            // Number of vertices per patch
            unsigned int mNrVertsPerPatch;
            // Number of patches along each direction
            unsigned int mNrPatches;

        public:
            // Constructor
            GLTerrainTessellated( int width, int height );

            // Function to render
            void draw() override;
    };

}

#endif
