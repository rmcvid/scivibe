#pragma once
#include "core/core.hpp"
#include "pch/pch.hpp"

namespace scivibe{
    struct FrameBufferSpecification{
        uint32_t Width, Height;
        uint32_t Sample = 1;
        bool SwapChainTarget = false; // glBindFrameBuffer(0);
    };

    class SCIVIBE_API FrameBuffer{
        public:
            FrameBuffer() = default;
            virtual ~FrameBuffer() = default;
            virtual const FrameBufferSpecification& GetSpecification( ) const=0;
            virtual void Bind() = 0 ;
            virtual void Unbind() =0;
            virtual void Resize(uint32_t width, uint32_t height) = 0;
            virtual void BindRead() = 0;
            virtual uint32_t GetColorAttachmentRendererID() const = 0;
            static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);

        private:
    };
}