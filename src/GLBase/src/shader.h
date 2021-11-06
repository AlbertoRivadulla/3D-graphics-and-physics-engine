#ifndef SHADER_H
#define SHADER_H

#include "GLBase.h"

namespace GLBase
{
    class Shader
    {
        public: 
            // The program ID
            unsigned int ID;

            // Constructor that reads and builds the shader from files.
            Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

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
}

#endif
