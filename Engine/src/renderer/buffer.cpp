#include "buffer.hpp"
#include "renderer/renderer.hpp"
#include "plateform/OpenGl/OpenGLBuffer.hpp"

namespace scivibe{
    Ref<VertexBuffer> VertexBuffer::Create(uint32_t size){
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: 
            SCIVIBE_ASSERT(false, "Renderer not supported");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLVertexBuffer>(size);
        }
        SCIVIBE_CORE_ASSERT(false, "Unknown render api");
    }   
    Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size){
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: 
            SCIVIBE_ASSERT(false, "Renderer not supported");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLVertexBuffer>(vertices,size);
        }
        SCIVIBE_CORE_ASSERT(false, "Unknown render api");
    }
    Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices,uint32_t count ){
        switch (Renderer::GetAPI()){
        case RendererAPI::API::None:    SCIVIBE_ASSERT(false, "Renderer not supported"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateRef<OpenGLIndexBuffer>(indices,count);
        }
        SCIVIBE_CORE_ASSERT(false, "Unknown render api");
    }   
}
