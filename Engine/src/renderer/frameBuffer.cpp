#include "renderer/frameBuffer.hpp"
#include "plateform/OpenGl/OpenGlFrameBuffer.hpp"
#include "renderer/renderer.hpp"
#include "renderer/renderer.hpp"


namespace scivibe{
    Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec){
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None : SCIVIBE_ASSERT(false, "Renderer::None n'est actuellement pas supportée"); return nullptr;
        case RendererAPI::API::OpenGL : return CreateRef<OpenGLFrameBuffer>(spec);
        }
        return nullptr;
    } 
}