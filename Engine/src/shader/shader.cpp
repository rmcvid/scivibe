#include "pch/pch.hpp"
#include "renderer/renderer.hpp"
#include "shader/shader.hpp"
#include "plateform/OpenGl/OpenGLShader.hpp"

namespace scivibe{
    Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc){
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: SCIVIBE_ASSERT(false, " Api non reconnu"); 
            return nullptr;        
        case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
        }
        SCIVIBE_ASSERT(false,"on ne devrait pas se retrouver là");
        return nullptr;
    } 
}