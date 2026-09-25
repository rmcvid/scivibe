#pragma once

#include "core/core.hpp"
#include "glm/glm.hpp"
#include "renderer/vertexArray.hpp"

namespace scivibe{
    class SCIVIBE_API RendererAPI{
        public:
            enum class API{
                None = 0, OpenGL =1
            };
            virtual void SetClearColor(const glm::vec4& color) = 0;
            virtual void Clear() = 0;
            virtual void DrawIndexed(const std::shared_ptr<VertexArray> vertexArray) = 0;
            inline static API GetAPI(){return s_API;}
        private:
            static API s_API;
            
    };
}
