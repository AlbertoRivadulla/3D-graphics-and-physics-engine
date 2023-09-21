#include "Terrain.h"
#include "GLElemObject.h"

using namespace GLBase;
using namespace GLGeometry;

namespace Physics
{
    // Constructor
    Terrain::Terrain( std::vector<GLElemObject*>* elementaryObjects ) :
        mElementaryObjects { elementaryObjects },
        mTessellationShader( Shader( std::string(BASE_DIR_SHADERS) + "/GLGeometry/tessellationGPassVertex.glsl",
                                     std::string(BASE_DIR_SHADERS) + "/GLGeometry/tessellationGPassFragment.glsl",
                                     "",
                                     std::string(BASE_DIR_SHADERS) + "/GLGeometry/tessellationGPassTessCtrl.glsl",
                                     std::string(BASE_DIR_SHADERS) + "/GLGeometry/tessellationGPassTessEval.glsl" ) )
    {
    }

    // Add a terrain patch from an image
    void Terrain::addPatchFromTexture( const std::string& heightmapTexPath, 
                                       float hScale, float vScale, float yShift )
    {
        // Load the texture for the height map
        int width, height, nChannels;
        unsigned char *data = stbi_load( heightmapTexPath.c_str(), &width,  
                                         &height, &nChannels, 0 );
        // Generate the mesh
        mTerrainPatch = new GLTerrainPatch( data, width, height, nChannels );
        stbi_image_free( data );

        // Add a model matrix to the GLTerrainPatch
        mTerrainPatch->setModelMatrix( { 0., yShift, 0. }, 0., {0., 0., 1.}, { hScale, vScale, hScale } );

        // // Add this to the list of elementary objects
        // // This is needed for it to be considered when computing the shadow maps
        // mElementaryObjects->push_back( mTerrainPatch );
    }

    // Add a terrain patch from an image
    void Terrain::addPatchFromTextureTessellated( const std::string& heightmapTexPath, 
                                                  float hScale, float vScale, float yShift )
    {
        glGenTextures(1, &mHeightmapTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mHeightmapTex); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load the texture for the height map
        int width, height, nChannels;
        unsigned char *data = stbi_load( heightmapTexPath.c_str(), &width,  
                                         &height, &nChannels, 0 );
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            mTessellationShader.use();
            mTessellationShader.setInt("heightMap", 0);
        }
        else
        {
            std::cout << "Failed to load the heightmap texture" << std::endl;
        }
        stbi_image_free(data);

        // Generate the base mesh.
        // This does not have the information of the height map
        mTerrainPatch = new GLTerrainTessellated( width, height );

        // Add a model matrix to the GLTerrainPatch
        mTerrainPatch->setModelMatrix( { 0., yShift, 0. }, 0., {0., 0., 1.}, { hScale, vScale, hScale } );

        // // Add this to the list of elementary objects
        // // This is needed for it to be considered when computing the shadow maps
        // mElementaryObjects->push_back( mTerrainPatch );

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Add material
    void Terrain::addMaterial( Material* material )
    {
        std::cout << "Terrain::addMaterial should add the material to only one of the terrain patches" << std::endl;
        std::cout << "NOTE" << std::endl;
        mMaterial = material;
    }

    // Get the tessellation shader
    Shader& Terrain::getTessellationShader()
    {
        return mTessellationShader;
    }

    // Draw the terrain
    void Terrain::draw()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mHeightmapTex);

        // This should select what patches to draw depending on the frustum

        // Configure the shader
        mMaterial->configShader( mTerrainPatch->getModelMatrix() );

        // Draw the object
        mTerrainPatch->draw();

        // std::cout << "draw the terrain patch " << std::endl;
        // std::cout << "NOTE" << std::endl;
    }
}
