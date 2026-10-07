#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include "glm/glm.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "core/log.hpp"

// from LearnOpenGL
namespace scivibe{
    class SCIVIBE_API Shader{
    public:
        // constructor generates the shader on the fly
        // ------------------------------------------------------------------------
        virtual ~Shader() = default;
        virtual void use() = 0;
        virtual void Bind() const = 0 ;
        virtual void UnBind() const = 0 ;
        virtual void SetInt(const std::string& name,const int value) const = 0;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count ) = 0;
        virtual void SetFloat(const std::string& name,const float value) = 0;
        virtual void SetFloat3(const std::string& name,const glm::vec3& value ) = 0;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value ) = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& value ) = 0;
        //virtual void UploadVSRendererUniformBuffer();
        

        virtual const std::string& GetName() const =0;
        static Ref<Shader> Create(const std::string& filePath);
        static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& geometrySrc = "");
    };

    class SCIVIBE_API ShaderLibrary{
        public :
            void Add(const std::string& name,const Ref<Shader>& shader);
            void Add(const Ref<Shader>& shader);
            Ref<Shader> Load(const std::string& path);
            Ref<Shader> Load(const std::string& name, const std::string& path);
            Ref<Shader> Load(const std::string& name, const std::string& pathvs, const std::string& pathfs, const std::string& pathgeo = "");
            Ref<Shader> Get(const std::string& name); 
            bool Exists(const std::string& name ) const ; 
        private:
            std::unordered_map<std::string, Ref<Shader>> m_Shaders;
    };
}
#endif
