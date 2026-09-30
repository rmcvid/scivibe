#pragma once

#include "renderer/vertexArray.hpp"

namespace scivibe{
    class OpenGLVertexArray : public VertexArray{
        public:
            OpenGLVertexArray();
            ~OpenGLVertexArray();
            virtual void Bind() const override;
            virtual void UnBind() const override ;
            virtual void AddVertexBuffer(const Ref<VertexBuffer> vertexBuffer) override;
            virtual void SetIndexBuffer(const Ref<IndexBuffer> indexBuffer) override;
            virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers()const {return m_VertexBuffer;} ;
            virtual const Ref<IndexBuffer>& GetIndexBuffers() const {return m_IndexBuffer;};
        private:
            std::vector<Ref<VertexBuffer>> m_VertexBuffer;
            Ref<IndexBuffer> m_IndexBuffer;
            uint32_t m_RendererID;
    };
}