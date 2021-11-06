#include "GLBase.h"

namespace GLBase
{
    // Constructor
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
         std::vector<Texture> textures) : vertices { vertices }, 
        indices { indices }, textures { textures }
    {
        // Initialize the mesh
        setupMesh();
    }

    // Method for rendering
    void Mesh::draw(Shader &shader)
    {
        // Set the textures in the shader.
        // Assume the naming convntion for the uniforms in the shader:
        //      texture_diffuseN
        //      texture_specularN
        // Where N the number of the texture, starting at 1
        // Create counters for the number of diffuse an specular textures
        unsigned int diffuseN { 1 };
        unsigned int specularN { 1 };
        unsigned int normalN { 1 };
        unsigned int heightN { 1 };
        // Add all the textures in the member vector to the shader
        for (int i = 0; i < textures.size(); ++i)
        {
            // Activate the proper texture unit before binding
            glActiveTexture(GL_TEXTURE0 + i);
            // Retrieve the texture number (the N above, inside the shader) and name
            std::string number;
            std::string name { textures[i].type };
            // Get the number for the uniforms in the shader
            if (name == "texture_diffuse")
                number = std::to_string(diffuseN++);
            if (name == "texture_specular")
                number = std::to_string(specularN++);
            if (name == "texture_normal")
                number = std::to_string(normalN++);
            if (name == "texture_height")
                number = std::to_string(heightN++);

            // Set the current texture locator to the corresponding uniform
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // Bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // Draw the mesh
        glBindVertexArray(VAO); // This also binds the corresponding EBO
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Set everything back to defaults.
        glActiveTexture(GL_TEXTURE0);
    }

    // Method for setting up the different buffers and specify the vertex shader
    // layout via vertex attribute pointers.
    void Mesh::setupMesh()
    {
        // Generate the vertex array object
        glGenVertexArrays(1, &VAO);
        // Generate the vertex buffer object and the element buffer object
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // Bind the VAO and the VBO (as a vertex buffer)
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Add the data to the VBO
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), 
                     &vertices[0], GL_STATIC_DRAW);

        // Bind the EBO as an element array buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // Add the data to the EBO
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                     &indices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // Vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // Vertex texture coordinates
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        // Unbind the VAO
        glBindVertexArray(0);
    }
}
