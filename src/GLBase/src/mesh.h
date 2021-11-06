#ifndef MESH_H
#define MESH_H

#include "GLBase.h"

namespace GLBase
{
    // Struct for a vertex in OpenGL
    struct Vertex 
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    // Struct to organize the texture data
    struct Texture
    {
        unsigned int id;
        // Type of the texture. Naming conventions:
        //      texture_diffuse
        //      texture_specular
        //      texture_normal
        //      texture_height
        std::string type;
        // The path of the texture file
        std::string path;
    };

    // Mesh class
    class Mesh
    {
        public:
            // Data for rendering
            unsigned int VAO;
            unsigned int VBO;
            unsigned int EBO;

            // Data of the mesh
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;

            // Constructor
            Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
                 std::vector<Texture> textures);

            // Method for rendering
            void draw(Shader &shader);

        private:
            // Method for setting up the different buffers and specify the vertex shader
            // layout via vertex attribute pointers.
            void setupMesh();
    };
}

#endif
