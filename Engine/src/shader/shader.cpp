#include "pch/pch.hpp"
#include "renderer/renderer.hpp"
#include "shader/shader.hpp"
#include "plateform/OpenGl/OpenGLShader.hpp"

namespace scivibe{
    Shader* Shader::Create(const std::string& filePath){
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: SCIVIBE_ASSERT(false, " Api non reconnu"); 
            return nullptr;        
        case RendererAPI::API::OpenGL: return new OpenGLShader(filePath);
        }
        SCIVIBE_ASSERT(false,"on ne devrait pas se retrouver là");
        return nullptr;
    } 
    Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& geometrySrc){
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: SCIVIBE_ASSERT(false, " Api non reconnu"); 
            return nullptr;        
        case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc, geometrySrc);
        }
        SCIVIBE_ASSERT(false,"on ne devrait pas se retrouver là");
        return nullptr;
    }

}