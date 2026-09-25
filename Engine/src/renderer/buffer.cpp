#include "buffer.hpp"
#include "renderer/renderer.hpp"
#include "plateform/OpenGl/OpenGLBuffer.hpp"

namespace scivibe{
    VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size){
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: 
            SCIVIBE_ASSERT(false, "Renderer not supported");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLVertexBuffer(vertices,size);
        }
        SCIVIBE_CORE_ASSERT(false, "Unknown render api");
    }   
    IndexBuffer* IndexBuffer::Create(uint32_t* indices,uint32_t count ){
        switch (Renderer::GetAPI()){
        case RendererAPI::API::None: 
            SCIVIBE_ASSERT(false, "Renderer not supported");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLIndexBuffer(indices,count);
        }
        SCIVIBE_CORE_ASSERT(false, "Unknown render api");
    }   
}
