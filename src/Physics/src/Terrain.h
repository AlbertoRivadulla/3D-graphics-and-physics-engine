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

            // Add a terrain patch from an image
            void addPatchFromTexture( const std::string& heightmapTexPath, float hScale, float vScale, float yShift=0.f);
            void addPatchFromTextureTessellated( const std::string& heightmapTexPath, float hScale, float vScale, float yShift=0.f);

            // Add material
            void addMaterial( Material* material );

            // Get the tessellation shader
            Shader& getTessellationShader();

            // Draw the terrain
            void draw();

        private:
            // List of elementary objects in the corresponding sandbox
            std::vector<GLElemObject*>* mElementaryObjects;

            // Heightmap texture
            unsigned int mHeightmapTex;

            // Material
            Material* mMaterial;

            // Terrain patch
            GLTerrainPatch* mTerrainPatch;

            // Shader for tessellation
            Shader mTessellationShader;
    };
}

#endif
