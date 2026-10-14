#include "renderer/texture.hpp"
#include "renderer/renderer.hpp"
#include "plateform/OpenGl/OpenGLTexture.hpp"
#include "core/core.hpp"
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

    SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max ) 
        : m_Texture(texture) {
        m_TexCoords[0] = {min.x,min.y};
        m_TexCoords[1] = {max.x,min.y};
        m_TexCoords[2] = {max.x,max.y};
        m_TexCoords[3] = {min.x,max.y};
    }
    /* sprite size donne la les dimension de la cellule en unité de l'image */
    Ref<SubTexture2D> SubTexture2D::createFromCoords(const Ref<Texture2D>& texture, glm::vec2& coords,  const glm::vec2& cellSize, const glm::vec2& spriteSize){
        glm::vec2 min = {(
            coords.x*cellSize.x)/ texture->GetWidth(),
            (coords.y*cellSize.y)/ texture->GetHeight()
        };
        glm::vec2 max = {
            ((coords.x+spriteSize.x)*cellSize.x)/ texture->GetWidth(),
            ((coords.y+spriteSize.y)*cellSize.y)/ texture->GetHeight()
        };
        return CreateRef<SubTexture2D>(texture,min,max);
    }
}