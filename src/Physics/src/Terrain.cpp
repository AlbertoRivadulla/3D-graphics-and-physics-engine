#include "Terrain.h"
#include "GLElemObject.h"
#include "utils.h"

using namespace GLBase;
using namespace GLGeometry;

namespace Physics
{
    // Constructor
    Terrain::Terrain( std::vector<GLElemObject*>* elementaryObjects ) :
        // mElementaryObjects { elementaryObjects },
        mTessellationShader( Shader( std::string(BASE_DIR_SHADERS) + "/GLGeometry/tessellationGPassVertex.glsl",
                                     std::string(BASE_DIR_SHADERS) + "/GLGeometry/tessellationGPassFragment.glsl",
                                     "",
                                     std::string(BASE_DIR_SHADERS) + "/GLGeometry/tessellationGPassTessCtrl.glsl",
                                     std::string(BASE_DIR_SHADERS) + "/GLGeometry/tessellationGPassTessEval.glsl" ) )
    {
    }

    // Destructor
    Terrain::~Terrain()
    {
        if ( mDataHeight )
            delete mDataHeight;
        if ( mDataNormal )
            delete mDataNormal;
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

    void Terrain::addPatchPlaneTessellated( float hScale, float vScale, float yShift)
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

        // The heightmap is a 2x2 texture whose values are all zero
        int width = 2;
        int height = 2;
        mDataHeight = new float[4];
        for ( int i = 0; i < 4; ++i )
            mDataHeight[i] = 0.f;

        // Write the data for the height map into the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, mDataHeight);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Compute the data for the normal map, and write it into its texture
        // computeNormalmapData( mDataHeight, width, height, hScale, vScale );
        computeNormalmapData( mDataHeight, width, height, hScale, vScale );

        mTessellationShader.use();
        mTessellationShader.setInt("heightMap", 0);
        mTessellationShader.setInt("normalMap", 1);

        // Generate the base mesh.
        // This does not have the information of the height map
        mTerrainPatch = new GLTerrainTessellated( width, height );

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
        unsigned char* data = stbi_load( heightmapTexPath.c_str(), &width,  
                                         &height, &nChannels, 0 );
        if (data)
        {
            // Create a single-channel texture
            mDataHeight = new float[ width*height ];

            // Copy the data to the float array, and find the maximum value
            float maxHeight = 0.f;
            for ( int i = 0; i < width; ++i )
            {
                for ( int j = 0; j < height; ++j )
                {
                    float accum = 0;
                    for ( int k = 0; k < nChannels; ++k )
                        accum += (float)data[ (j*width + i)*nChannels + k ];
                    mDataHeight[ (j*width + i) ] = accum / (float)nChannels;

                    if ( mDataHeight[ (j*width + i) ] > maxHeight )
                        maxHeight = mDataHeight[ (j*width + i) ];
                }
            }
            // Normalize the heightmap
            for ( int i = 0; i < width*height; ++i )
                mDataHeight[i] /= maxHeight;

            // Write the data for the height map into the texture
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mDataHeight);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, mDataHeight);
            glGenerateMipmap(GL_TEXTURE_2D);

            // Compute the data for the normal map, and write it into its texture
            computeNormalmapData( mDataHeight, width, height, hScale, vScale );

            mTessellationShader.use();
            mTessellationShader.setInt("heightMap", 0);
            mTessellationShader.setInt("normalMap", 1);
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
    }

    void Terrain::addPatchFromHeightDataTessellated( float* heightMapData, 
                                                     int width, int height,
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

        mDataHeight = heightMapData;

        // Normalize the heightmap
        float maxHeight = 0.f;
        for ( int i = 0; i < width*height; ++i )
            if (mDataHeight[i] > maxHeight)
                maxHeight = mDataHeight[i];
        for ( int i = 0; i < width*height; ++i )
            mDataHeight[i] /= maxHeight;

        // Write the data for the height map into the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, mDataHeight);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Compute the data for the normal map, and write it into its texture
        // computeNormalmapData( mDataHeight, width, height, hScale, vScale );
        computeNormalmapData( mDataHeight, width, height, hScale, vScale );

        mTessellationShader.use();
        mTessellationShader.setInt("heightMap", 0);
        mTessellationShader.setInt("normalMap", 1);

        // Generate the base mesh.
        // This does not have the information of the height map
        mTerrainPatch = new GLTerrainTessellated( width, height );

        // Add a model matrix to the GLTerrainPatch
        mTerrainPatch->setModelMatrix( { 0., yShift, 0. }, 0., {0., 0., 1.}, { hScale, vScale, hScale } );

        // // Add this to the list of elementary objects
        // // This is needed for it to be considered when computing the shadow maps
        // mElementaryObjects->push_back( mTerrainPatch );
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
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mNormalmapTex);

        // This should select what patches to draw depending on the frustum

        // Configure the shader
        mMaterial->configShader( mTerrainPatch->getModelMatrix() );

        // Draw the object
        mTerrainPatch->draw();

        // std::cout << "draw the terrain patch " << std::endl;
        // std::cout << "NOTE" << std::endl;
    }

    // Compute the normal map given an array of data for the height map
    void Terrain::computeNormalmapData( float* data, int width, int height,
                                        float hScale, float vScale )
    {
        mDataNormal = new float[ width * height * 4 ];

        // Compute the normals from the heightmap
        // https://stackoverflow.com/questions/49640250/calculate-normals-from-heightmap
        glm::vec3 normal = { 0.f, 1.f, 0.f };

        // Set the corners and edges to have vertical normal
        for ( int i = 0; i < width; ++i )
        {
            for ( int k = 0; k < 3; ++k )
            {
                mDataNormal[ i*4 + k ] = normal[k];
                mDataNormal[ ((height-1)*width + i)*4 + k ] = normal[k];
            }
        }
        for ( int j = 0; j < height; ++j )
        {
            for ( int k = 0; k < 3; ++k )
            {
                mDataNormal[ (j*width)*4 + k ] = normal[k];
                mDataNormal[ (j*width + height-1)*4 + k ] = normal[k];
            }
        }

        // Rest of the grid
        for ( int i = 2; i < width - 2; ++i )
        {
            for ( int j = 2; j < height - 2; ++j )
            {
                // normal[0] = 0.5f * vScale/hScale * ( (float)data[ (j*width + i-1) ] - (float)data[ (j*width + i+1) ] );
                // normal[1] = 1.f;
                // normal[2] = 0.5f * vScale/hScale * ( (float)data[ ((j-1)*width + i) ] - (float)data[ ((j+1)*width + i) ] );
                // normal = glm::normalize(normal);

                normal[0] = 1/12.f * vScale/hScale * ( - data[ (j*width + i-2) ] 
                                                       + 8.f * data[ (j*width + i-1) ] 
                                                       - 8.f * data[ (j*width + i+1) ] 
                                                       + data[ (j*width + i+2) ] 
                                                     );
                normal[1] = 1.f;
                normal[2] = 1/12.f * vScale/hScale * ( - data[ ((j-2)*width + i) ] 
                                                       + 8.f * data[ ((j-1)*width + i) ] 
                                                       - 8.f * data[ ((j+1)*width + i) ] 
                                                       + data[ ((j+2)*width + i) ] 
                                                     );
                normal = glm::normalize(normal);

                for ( int k = 0; k < 3; ++k )
                    mDataNormal[ (j*width + i)*4 + k ] = normal[k];
            }
        }

        // Setup the texture for the normal map
        glGenTextures(1, &mNormalmapTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mNormalmapTex);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Pass the data to the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, mDataNormal);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}
