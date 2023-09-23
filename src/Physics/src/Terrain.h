#ifndef TERRAIN_H
#define TERRAIN_H

#include "GLBase.h"
#include "GLGeometry.h"
#include "Colliders.h"
#include "PhysicsBody.h"

using namespace GLGeometry;
using namespace GLBase;

namespace Physics
{
    class Terrain
    {
        public:
            // Constructor
            Terrain( std::vector<GLElemObject*>* elementaryObjects );
            // Destructor
            ~Terrain();

            // Add a terrain patch from an image
            void addPatchFromTexture( const std::string& heightmapTexPath, 
                                      float hScale, float vScale, float yShift=0.f);
            void addPatchPlaneTessellated( float hScale, float vScale, float yShift=0.f);
            void addPatchFromTextureTessellated( const std::string& heightmapTexPath, 
                                                 float hScale, float vScale, float yShift=0.f);
            void addPatchFromHeightDataTessellated( float* heightMapData,
                                                    int width, int height, float hScale, 
                                                    float vScale, float yShift=0.f);

            // Add material
            void addMaterial( Material* material );

            // Get the tessellation shader
            Shader& getTessellationShader();

            // Draw the terrain
            void draw();

        private:
            // // List of elementary objects in the corresponding sandbox
            // std::vector<GLElemObject*>* mElementaryObjects;

            // Height map and Normal map texture
            unsigned int mHeightmapTex;
            unsigned int mNormalmapTex;
            // Data for the height and normal maps
            float* mDataHeight;
            float* mDataNormal;

            // Material
            Material* mMaterial;

            // Terrain patch
            GLTerrainPatch* mTerrainPatch;

            // Shader for tessellation
            Shader mTessellationShader;

            // Compute the normal map given an array of data for the height map
            void computeNormalmapData( float* data, int width, int height,
                                       float hScale, float vScale );
    };
}

#endif
