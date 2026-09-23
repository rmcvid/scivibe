#pragma once

#include "renderer/vertexArray.hpp"

namespace scivibe{
    class OpenGLVertexArray : public VertexArray{
        public:
            OpenGLVertexArray();
            ~OpenGLVertexArray();
            virtual void Bind() const override;
            virtual void UnBind() const override ;
            virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer> vertexBuffer) override;
            virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer> IndexBuffer) override;
            virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers()const {return m_VertexBuffer;} ;
            virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffers() const {return m_IndexBuffer;};
        private:
            std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffer;
            std::shared_ptr<IndexBuffer> m_IndexBuffer;
            uint32_t m_RendererID;
    };
}