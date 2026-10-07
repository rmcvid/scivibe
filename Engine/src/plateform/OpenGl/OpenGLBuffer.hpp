#pragma once
#include "renderer/buffer.hpp"
#include "glad/glad.h"
#include "glfw/glfw3.h"

namespace scivibe{
    class OpenGLVertexBuffer: public VertexBuffer
    {
        public:
            OpenGLVertexBuffer(uint32_t size);
            OpenGLVertexBuffer(float* vertices, uint32_t size);
            ~OpenGLVertexBuffer() override ;
            virtual void Bind() const;
            virtual void UnBind() const;
            virtual const BufferLayout& GetLayout() const override{return m_Layout;}
            virtual void SetLayout(const BufferLayout& layout) override{ m_Layout = layout;}
            virtual void SetData(const void* data, uint32_t size ) override;

        private : 
            uint32_t m_RendererID;
            BufferLayout m_Layout;
    };

    class OpenGLIndexBuffer: public IndexBuffer
    {
        public:
            OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
            ~OpenGLIndexBuffer() override;
            virtual void Bind() const;
            virtual void UnBind() const;
            virtual uint32_t GetCount() const{return m_Count;}
        private : 
            uint32_t m_RendererID;
            uint32_t m_Count;
    };
}