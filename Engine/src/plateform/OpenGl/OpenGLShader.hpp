#pragma once
#include <glad/glad.h>
#include "glm/glm.hpp"
#include <string>
#include "shader/shader.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "core/log.hpp"

// from LearnOpenGL
namespace scivibe{
    class SCIVIBE_API OpenGLShader : public Shader{
    public:
        // constructor generates the shader on the fly
        // ------------------------------------------------------------------------
        OpenGLShader(const std::string& filePath);
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& geometrySrc = "");
        ~OpenGLShader();
        void use() override;
        void Bind() const override;
        void UnBind() const override;


        void setBool(const std::string &name, bool value) const;
        void SetInt(const std::string &name, int value) const override;
        void SetIntArray(const std::string& name, int* values, uint32_t count ) override;
        void UploadUniformInt(const std::string &name, const int value) const;
        void UploadUniformFloat(const std::string &name, const float value) const;
        void UploadUniformFloat2(const std::string &name, const glm::vec2& value) const;
        void UploadUniformFloat3(const std::string &name, const glm::vec3& value) const;
        void UploadUniformFloat4(const std::string &name, const glm::vec4& value) const;
        void SetUniformMat3(const std::string& name, const glm::mat3& mat);
        void SetUniformMat4(const std::string& name, const glm::mat4& mat);

        virtual void SetFloat(const std::string& name, const float value) override;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value ) override;
        virtual void SetFloat4(const std::string& name,const glm::vec4& value ) override;
        virtual void SetMat4(const std::string& name, const glm::mat4& value )   override;

        

        virtual const std::string& GetName() const override {return m_Name;};

    private:
        uint32_t m_ID = 0;
        std::string m_Name;
        std::string ReadFile(const std::string& filePath);
        std::unordered_map<GLenum,std::string> PreProcess(const std::string& source);
        void Compile(const std::unordered_map<GLenum,std::string>& ShaderSrc);
    };
}
