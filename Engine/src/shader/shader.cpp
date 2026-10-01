#include "pch/pch.hpp"
#include "renderer/renderer.hpp"
#include "shader/shader.hpp"
#include "plateform/OpenGl/OpenGLShader.hpp"

namespace scivibe{
    Ref<Shader> Shader::Create(const std::string& filePath){
        switch (Renderer::GetAPI()){
        case RendererAPI::API::None: SCIVIBE_ASSERT(false, " Api non reconnu"); 
            return nullptr;        
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(filePath);
        }
        SCIVIBE_ASSERT(false,"on ne devrait pas se retrouver là");
        return nullptr;
    } 
    Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& geometrySrc){
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: SCIVIBE_ASSERT(false, " Api non reconnu"); 
            return nullptr;        
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc, geometrySrc);
        }
        SCIVIBE_ASSERT(false,"on ne devrait pas se retrouver là");
        return nullptr;
    }

    void ShaderLibrary::Add(const std::string& name ,const Ref<Shader>& shader){
        SCIVIBE_CORE_ASSERT(!Exists(name), "Shader already exist")
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Add(const Ref<Shader>& shader){
        auto& name = shader->GetName();
        Add(name, shader);
    }
    Ref<Shader> ShaderLibrary::Load(const std::string& path){
        auto shader = Shader::Create(path);
        Add(shader);
        return shader;
    }
    Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& path){
        auto shader = Shader::Create(path);
        Add(name, shader);
        return shader;
    }
    Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& pathvs, const std::string& pathfs, const std::string& pathgeo ){
        auto shader = Shader::Create(name,pathvs,pathfs,pathgeo);
        Add(name, shader);
        return shader;
    }
    Ref<Shader> ShaderLibrary::Get(const std::string& name){
        SCIVIBE_CORE_ASSERT(Exists(name), "Shader not found")
        return m_Shaders[name];
    } 
    bool ShaderLibrary::Exists(const std::string& name ) const {
        return m_Shaders.find(name) != m_Shaders.end();
    }

}