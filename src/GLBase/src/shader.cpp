#include "GLBase.h"

namespace GLBase
{
    // Constructor that reads and builds the shader from files.
    // Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
    {
        // Read the shaders from the files.

        // Variables to hold the source read from the files given.
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        // Input streams to operate on the files.
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // Ensure that the input streams can throw exceptions.
        // This is done by means of a mask, that determines the conditions
        // on which they can throw exceptions..
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            // Open the files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            // Read the files' contents into streams.
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // Close the file handlers.
            vShaderFile.close();
            fShaderFile.close();
            // Convert the streams into strings.
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            
            // If the geometry path is present, load the geometry shader too
            if (geometryPath != "")
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << geometryPath << std::endl;
        }
        const char* vShaderCode { vertexCode.c_str() };
        const char* fShaderCode { fragmentCode.c_str() };

        // Compile the shaders.
        unsigned int vertex, fragment, geometry;
        int success;
        char infoLog[512]; // Container for error messages.

        // Create the vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        // Attach the shader source code to it, and compile.
        // The second argument specifies how many string of source code we are passing.
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // If compilation failed, retrieve the error message.
        checkCompileErrors(vertex, "VERTEX");
            
        // Create the fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        // Attach the shader source code to it, and compile.
        // The second argument specifies how many string of source code we are passing.
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        // If compilation failed, retrieve the error message.
        checkCompileErrors(fragment, "FRAGMENT");

        // If the geometry shader is given, compile it
        if (geometryPath != "")
        {
            const char* gShaderCode { geometryCode.c_str() };
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }

        // Create the shader program
        ID = glCreateProgram();
        // Attach the shaders to the program, and link them
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometryPath != "")
            glAttachShader(ID, geometry);
        glLinkProgram(ID); 
        // Check for linking errors
        checkCompileErrors(ID, "PROGRAM");

        // Once the shaders are linked in the program, we don't need the shader objects.
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != "")
            glDeleteShader(geometry);
    }

    // Use/activate the shader
    void Shader::use()
    {
        glUseProgram(ID);
    }

    // Utility uniform functions
    void Shader::setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void Shader::setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void Shader::setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void Shader::setFloat3(const std::string &name, float value1, float value2, float value3) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3);
    }
    void Shader::setFloat4(const std::string &name, float value1, float value2, float value3, float value4) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3, value4);
    }
    // ------------------------------------------------------------------------
    void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void Shader::setVec2(const std::string &name, const glm::vec2 &vec) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &vec[0]);
    }
    void Shader::setVec3(const std::string &name, const glm::vec3 &vec) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &vec[0]);
    }

    // Utility function for checking compile errors for the shaders
    void Shader::checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            // If compilation failed, retrieve the error message.
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type
                    << '\n' << infoLog << '\n';
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type
                    << '\n' << infoLog << '\n';
            }
        }
    }
}
