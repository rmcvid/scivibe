#pragma once
#include "core/core.hpp"
#include "pch/pch.hpp"
#include "renderer/frameBuffer.hpp"

namespace scivibe{
    class OpenGLFrameBuffer : public FrameBuffer
    {
        public:
            OpenGLFrameBuffer(const FrameBufferSpecification& spec);
            ~OpenGLFrameBuffer();
            void Invalidate();
            virtual void Bind() override;
            virtual void Unbind() override;
            virtual void Resize(uint32_t width, uint32_t height) override;

            //virtual uint32_t GetColorAttachmentRendererID() const override {return m_ColorAttachment;};
            virtual const FrameBufferSpecification& GetSpecification()const override { return m_Spec;};
            uint32_t GetColorAttachmentRendererID() const override {return m_ResolveColorAttachment;}
            void BindRead() override;
        private:
            uint32_t m_RendererID = 0;
            uint32_t m_ColorAttachment = 0;
            uint32_t m_DepthAttachment = 0;

            uint32_t m_ResolveFramebuffer = 0;
            uint32_t m_ResolveColorAttachment = 0;

            FrameBufferSpecification m_Spec;
    };
}