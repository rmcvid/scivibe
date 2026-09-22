#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "log/log.hpp"

// from LearnOpenGL
namespace scivibe{
    class SCIVIBE_API Shader{
    public:
        unsigned int m_ID;
        // constructor generates the shader on the fly
        // ------------------------------------------------------------------------
        Shader(const char* vertexPath, const char* fragmentPath);
        ~Shader();
        void use();
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void Bind() const;
        void UnBind() const;

    private:
        void checkCompileErrors(unsigned int shader, std::string type){
            int success;
            char infoLog[1024];
            if (type != "PROGRAM")
            {
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                    SCIVIBE_CORE_ERROR("ERROR::SHADER_COMPILATION_ERROR of type: {0} {1} ", type, infoLog);
                    //std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                }
            }
            else
            {
                glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success)
                {
                    glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                    SCIVIBE_CORE_ERROR("ERROR::SHADER_COMPILATION_ERROR of type: {0} {1} ", type, infoLog);
                    //std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
                }
            }
        }
    };
}
#endif
