#pragma once

#include "core/core.hpp"
#include "renderer/buffer.hpp"

namespace scivibe{
    class SCIVIBE_API VertexArray{
        public:
        virtual ~VertexArray(){}
        virtual void Bind() const =0;
        virtual void UnBind() const =0;

        virtual void AddVertexBuffer(const Ref<VertexBuffer> vertexBuffer) = 0;
        virtual void SetIndexBuffer(const Ref<IndexBuffer> IndexBuffer) =0;
        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
        virtual const Ref<IndexBuffer>& GetIndexBuffers() const = 0;

        static Ref<VertexArray> Create();

        
    };
}
