#include "GLBase.h"

namespace GLBase
{
    //====================
    // Methods of the Model class
    //====================

    // Constructor
    Model::Model(const std::string& path, bool gamma) : gammaCorrection { gamma }
    {
        std::cout << "Loading model...\n";
        // Load the model from the path given
        loadModel(path);
        std::cout << "Model loaded.\n";
    }

    // Draw function
    void Model::draw(Shader& shader)
    {
        // Draw all the meshes in the model
        for (int i = 0; i < meshes.size(); ++i)
            meshes[i].draw(shader);
    }

    // Function to load a model using Assimp's functions
    void Model::loadModel(std::string path)
    {
        // Declare an importer from Assimp
        Assimp::Importer importer;
        // Read the model from a path with several directives, some of which are:
        //      aiProcess_Triangulate: convert the entire mesh to triangles if they are not
        //      aiProcess_FlipUVs: flip the texture coordinates on the y axis
        //      aiProcess_GetNormals: create normal vectors for each vertex if the model does not contain them
        //      aiProcess_SplitLargeMeshes: split the mesh into smaller ones
        //      aiProcess_OptimizeMeshes: join several meshes into larger ones, to reduce draw calls
        const aiScene* scene { importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs) };

        // Check if the scene was loaded properly
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        // Retrieve the directory path of the given file path
        directory = path.substr(0, path.find_last_of('/'));

        // Pass the first node to the recursive processNode function
        processNode(scene->mRootNode, scene);
    }


    // Method to recursively process each node and their children for a loaded model
    // Each node contains a set of mesh indices, that point to a particular mesh of the scene
    void Model::processNode(aiNode* node, const aiScene* scene)
    {
        // Process all the node's meshes, if any
        for (int i = 0; i < node->mNumMeshes; ++i)
        {
            // Retrieve the mesh
            aiMesh* mesh { scene->mMeshes[node->mMeshes[i]] };
            // Save it in the list of meshes of the model, after doing some processing on it
            // with the member function processMesh.
            meshes.push_back(processMesh(mesh, scene));
        }
        // Repeat recurrently for the childen nodes
        for (int i = 0; i < node->mNumChildren; ++i)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    // Method to translate the aiMesh objects into an instance of our class mesh
    Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // Initialize variables
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // Process the vertices
        for (int i = 0; i < mesh->mNumVertices; ++i)
        {
            // Define a blank vertex first
            Vertex vertex;

            // Get the vertex position
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // Get the vertex normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;

            // Get the texture coordinates, if they exist
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            // Store the vertex
            vertices.push_back(vertex);
        }

        // Process the indices
        // Iterate through the faces (since we specified aiProcess_Triangulate when
        // reading the mesh, these will be triangles)
        for (int i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face { mesh->mFaces[i] };
            // Read all the vertices in the face, and store them
            for (int j = 0; j < face.mNumIndices; ++j)
            {
                // Store each index in sequence
                indices.push_back(face.mIndices[j]);
            }
        }

        // Process the material
        // A mesh can contain an index to a material object from the scene
        if (mesh->mMaterialIndex >= 0)
        {
            // Retrieve the material object from the scene
            aiMaterial* material { scene->mMaterials[mesh->mMaterialIndex] };

            // Load the diffuse textures
            std::vector<Texture> diffuseMaps { loadMaterialTextures(material, aiTextureType_DIFFUSE,
                                                               "texture_diffuse") };
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // Load the specular textures
            std::vector<Texture> specularMaps { loadMaterialTextures(material, aiTextureType_SPECULAR,
                                                               "texture_specular") };
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // Load the normal map textures
            std::vector<Texture> normalMaps { loadMaterialTextures(material, aiTextureType_HEIGHT,
                                                               "texture_normal") };
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // Load the height map textures
            std::vector<Texture> heightMaps { loadMaterialTextures(material, aiTextureType_AMBIENT,
                                                               "texture_height") };
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        // Return the created mesh object
        return Mesh(vertices, indices, textures);
    }

    // Load all the textures of a determined type in the given material
    std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type,
                                              std::string typeName)
    {
        std::vector<Texture> textures;
        // Iterate through all the textures of the given type
        for (int i = 0; i < material->GetTextureCount(type); ++i)
        {
            // Variable for the path of the texture
            aiString str;
            // Get the path of the current texture
            material->GetTexture(type, i, &str);
            // Check if the texture was loaded before
            bool skip { false };
            for (int j = 0; j < texturesLoaded.size(); ++j)
            {
                // Check if the current texture is already in the vector of textures loaded
                if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0)
                {
                    // Add the loaded texture to the vector of textures
                    textures.push_back(texturesLoaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                // If the texture has not been loaded yet, load it now
                Texture texture;
                texture.id = loadTextureFromDirectory(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                // Store it also in the vector of loaded textures
                texturesLoaded.push_back(texture);
            }
        }
        return textures;
    }

    // Function to load a texture with a path inside a directory
    unsigned int loadTextureFromDirectory(const char* path, const std::string& directory, bool gamma)
    {
        // Add the directory to the path of the string
        std::string pathWithDirectory { std::string(path) };
        pathWithDirectory = directory + '/' + pathWithDirectory;

        return loadTexture(pathWithDirectory.c_str());
    }

    // Function to load a texture
    unsigned int loadTexture(const char *path, bool sRGB, bool hasAlpha)
    {
        // Generate the texture
        unsigned int textureID;
        glGenTextures(1, &textureID);

        // Load the textures using the functions defined in stb_image.h
        int width, height, nrChannels;
        // By default, the image will be flipped vertically, since the (0,0) point in 
        // an image is at the upper left, while for OpenGL it is at the lower left. 
        // Fix this
        stbi_set_flip_vertically_on_load(true);
        // Load the first texture
        unsigned char* data { stbi_load(path, &width, &height, &nrChannels, 0) };
        // Check if the data was loaded successfully
        if (data)
        {
            // Get the format of the texture
            GLenum colorFormatIn;
            GLenum colorFormatOut;
            switch (nrChannels)
            {
                case 1:
                    colorFormatOut = GL_RED;
                    colorFormatIn = GL_RED;
                    break;
                case 3:
                    colorFormatOut = GL_RGB;
                    if (sRGB)
                        colorFormatIn = GL_SRGB;
                    else
                        colorFormatIn = GL_RGB;
                    break;
                case 4:
                    colorFormatOut = GL_RGBA;
                    if (sRGB)
                        colorFormatIn = GL_SRGB_ALPHA;
                    else
                        colorFormatIn = GL_RGBA;
                    break;
            }

            // Bind the generated texture to its corresponding type
            glBindTexture(GL_TEXTURE_2D, textureID);
            // Generate the texture on the currently bound texture instance.
            // The 2nd argument is the level of the mipmap on which to generate it.
            // The 3rd argument is the format on which we want to store the texture.
            // The 6th argument must be 0 (legacy).
            // The 7th argument specifies the format of the source.
            // The 8th argument specifies the datatype of the source. In this case
            // unsigned chars, which are bytes.
            glTexImage2D(GL_TEXTURE_2D, 0, colorFormatIn, width, height, 0, colorFormatOut, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            if (hasAlpha)
            {
                // Set the option to clamp to edge
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            }
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            std::cout << "Failed to load the texture.\n";
        }
        stbi_image_free(data);
         
        // Reture the ID of the generated texture
        return textureID;
    }

    // Function to load a cubemap
    unsigned int loadCubemap(std::vector<std::string> faces)
    {
        // Create a texture and bind it to GL_TEXTURE_CUBE_MAP
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        // Load the texture for each face of the cube.
        // Start with GL_TEXTURE_CUBE_MAP_POSITIVE_X (right face), and increment it 
        // by one for each step. The order is:
        //  right - left - top - bottom - back - front
        int width, height, nrChannels;
        unsigned char* data;
        // The textures should not be flipped, since cubemap images are expected to 
        // start at the top left, instead of the bottom left.
        stbi_set_flip_vertically_on_load(false);
        // Iterate over the faces
        for (int i = 0; i < faces.size(); ++i)
        {
            // Load the data in each face
            data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            // Generate the texture from the loaded data, in the corresponding face
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                         0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        // Specify the wrapping and filtering methods for the cubemap texture.
        // The R dimension corresponds to the tird dimension, z.
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return textureID;
    }
}
