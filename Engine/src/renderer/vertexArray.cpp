#include "renderer/vertexArray.hpp"
#include "renderer/renderer.hpp"
#include "plateform/OpenGl/OpenGLVertexArray.hpp"


namespace scivibe{
    VertexArray* VertexArray::Create(){
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: SCIVIBE_ASSERT(false, " Api non reconnu"); 
            return nullptr;        
        case RendererAPI::API::OpenGL: return new OpenGLVertexArray();
        }
        SCIVIBE_ASSERT(false,"on ne devrait pas se retrouver là");
        return nullptr;
    } 
}