#ifndef MODEL_H
#define MODEL_H

#include "GLBase.h"

// Header needed to import images for textures
#include <stb_image.h>
// Headers needed to import a model
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace GLBase
{
    class Model
    {
        public:
            // Model data
            std::vector<Mesh> meshes;
            std::string directory;
            std::vector<Texture> texturesLoaded;
            // Bool to know if we have to perform gamma correction or not
            bool gammaCorrection;

            // Constructor
            Model(const std::string& path, bool gamma = false);

            // Draw function
            void draw(Shader& shader);

        private:

            // Functions to process Assimp's import routine
            // Function to load a model using Assimp's functions
            void loadModel(std::string path);
            // Method to recursively process each node and their children for a loaded model
            void processNode(aiNode* node, const aiScene* scene);
            // Method to translate the aiMesh objects into an instance of our class mesh
            Mesh processMesh(aiMesh* mesh, const aiScene* scene);
            std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type,
                                                      std::string typeName);
    };


    // Function to load a texture with a path inside a directory
    unsigned int loadTextureFromDirectory(const char* path, const std::string& directory, bool gamma = false);

    // Function to load a texture
    unsigned int loadTexture(const char *path, bool sRGB = false, bool hasAlpha = false);

    // Function to load a cubemap
    unsigned int loadCubemap(std::vector<std::string> faces);
}

#endif
