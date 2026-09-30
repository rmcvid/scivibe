#pragma once
#include <glad/glad.h>
#include "glm/glm.hpp"
#include <string>
#include "shader/shader.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "log/log.hpp"

// from LearnOpenGL
namespace scivibe{
    class SCIVIBE_API OpenGLShader : public Shader{
    public:
        // constructor generates the shader on the fly
        // ------------------------------------------------------------------------
        OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~OpenGLShader();
        void use() override;
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void UploadUniformInt(const std::string &name, const int value) const;

        void UploadUniformFloat(const std::string &name, const float value) const;
        void UploadUniformFloat2(const std::string &name, const glm::vec2& value) const;
        void UploadUniformFloat3(const std::string &name, const glm::vec3& value) const;
        void UploadUniformFloat4(const std::string &name, const glm::vec4& value) const;

        void setUniformMat3(const std::string& name, const glm::mat3& mat);
        void setUniformMat4(const std::string& name, const glm::mat4& mat);
        void Bind() const override;
        void UnBind() const override;

    private:
        uint32_t m_ID;
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
