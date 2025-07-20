#ifndef SHADER_H
#define SHADER_H

// Header needed to import images for textures
#include <stb_image.h>
#include "GLBase.h"

namespace GLBase
{
    class Shader
    {
        public: 
            // The program ID
            unsigned int ID;

            // Constructor that reads and builds the shader from files.
            // Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");
            Shader(const std::string& vertexPath, const std::string& fragmentPath, 
                   const std::string& geometryPath = "",
                   const std::string& tessCtrlPath = "", const std::string& tessEvalPath = "");

            // Use/activate the shader
            void use();

            // Utility uniform functions
            void setBool(const std::string &name, bool value) const;
            void setInt(const std::string &name, int value) const;
            void setFloat(const std::string &name, float value) const;
            // ------------------------------------------------------------------------
            void setFloat3(const std::string &name, float value1, float value2, 
                           float value3) const;
            void setFloat4(const std::string &name, float value1, float value2, 
                           float value3, float value4) const;
            // ------------------------------------------------------------------------
            void setVec2(const std::string &name, const glm::vec2 &vec) const;
            void setVec3(const std::string &name, const glm::vec3 &vec) const;
            // ------------------------------------------------------------------------
            void setMat2(const std::string &name, const glm::mat2 &mat) const;
            void setMat3(const std::string &name, const glm::mat3 &mat) const;
            void setMat4(const std::string &name, const glm::mat4 &mat) const;

        private:
            // Utility function for checking compile errors for the shaders
            void checkCompileErrors(GLuint shader, std::string type);
    };

    struct Material
    {
        // Properties
        glm::vec3 albedo;
        float spec;
        float emissive;

        // Pointer to a shader
        Shader* shader;

        // Constructor
        Material( Shader& shaderRef, glm::vec3 alb, float specular, float emiss = 0 )
        {
            shader = &shaderRef;

            albedo = alb;
            spec = specular;
            emissive = emiss;
        }

        // Destructor
        virtual ~Material()
        {}

        // Method to pass the material information to a shader.
        // void configShader(const Shader& shader)
        virtual void configShader( const glm::mat4& modelMatrix )
        {
            shader->use();
            shader->setMat4( "model", modelMatrix );

            shader->setVec3("material.albedo", albedo);
            shader->setFloat("material.spec", spec);
            shader->setFloat("material.emissive", emissive);
        }
    };

    struct MaterialWithTextures : Material
    {
        // Boolean values if it has terxtures
        bool hasTexAlbedo;
        bool hasTexNormal;

        // Textures
        unsigned int texAlbedo;
        unsigned int texNormal;

        // Constructor
        MaterialWithTextures( Shader& shaderRef, glm::vec3 alb, float specular, 
                              float emiss = 0 ) : Material( shaderRef, alb, specular, emiss )
        {
            // hasTexAlbedo = false;
            // hasTexNormal = false;
            //
            // // Set the location of the textures in the shader
            // shader->use();
            // shader->setInt( "material.texAlbedo", 0 );
            // shader->setInt( "texNormal", 1 );
        }

        unsigned int loadTexture( const char *path, bool sRGB=false, bool hasAlpha=false )
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
                LOG_ERROR("Failed to load the texture.");
            }
            stbi_image_free(data);
             
            // Reture the ID of the generated texture
            return textureID;
        }

        // Methods to load the textures
        void loadAlbedoTexture( const std::string& path, bool sRGB=false, bool hasAlpha=false )
        {
            hasTexAlbedo = true;
            texAlbedo = loadTexture( path.c_str(), sRGB, hasAlpha );
        }
        void loadNormalMapTexture( const std::string& path, bool sRGB=false, bool hasAlpha=false )
        {
            hasTexNormal = true;
            texNormal = loadTexture( path.c_str(), sRGB, hasAlpha );

            LOG_WARNING("The logic for a normal map is still not implemented in the shader!");
            LOG_WARNING("This would require the computation beforehand of the tangent and bitangent vectors of the object");
        }

        // Method to pass the material information to a shader.
        // This overrides the one in the base class
        void configShader( const glm::mat4& modelMatrix ) override
        {
            shader->use();
            shader->setMat4( "model", modelMatrix );

            // Bind the textures, if they exist
            if ( hasTexAlbedo )
            {
                // shader->setBool("material.hasTexAlbedo", true);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texAlbedo);
            }
            else
                shader->setVec3( "material.albedo", albedo );
            if ( hasTexNormal )
            {
                // shader->setBool("hasTexNormal", true);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texNormal);
            }
            shader->setBool("material.hasTexAlbedo", hasTexAlbedo);
            shader->setBool("hasTexNormal", hasTexNormal);
        }
    };
}

#endif
