#include "pch/pch.hpp"
#include "renderCommand.hpp"
#include "plateform/OpenGl/OpenGLRendererAPI.hpp"

namespace scivibe{
    RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
    
}