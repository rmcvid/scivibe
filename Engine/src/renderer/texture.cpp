#include "renderer/texture.hpp"
#include "renderer/renderer.hpp"
#include "plateform/OpenGl/OpenGLTexture.hpp"
namespace scivibe{
    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height){
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: SCIVIBE_ASSERT(false, " Api non reconnu"); 
            return nullptr;        
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width,height);
        }
        SCIVIBE_ASSERT(false,"on ne devrait pas se retrouver là");
        return nullptr;
    }
    Ref<Texture2D> Texture2D::Create(const std::string& path){
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: SCIVIBE_ASSERT(false, " Api non reconnu"); 
            return nullptr;        
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);
        }
        SCIVIBE_ASSERT(false,"on ne devrait pas se retrouver là");
        return nullptr;
    }
}