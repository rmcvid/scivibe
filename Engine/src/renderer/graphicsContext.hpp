#pragma once

#include "core/log.hpp"
#include "core/capturedFrame.hpp"
#include "renderer/frameBuffer.hpp"
namespace scivibe {
    class GraphicsContext{
        public:
            virtual void Init() =0;
            virtual void SwapBuffers()=0 ;
            virtual bool CaptureFrame(CapturedFrame& frame) = 0;
            virtual bool CaptureFrame(CapturedFrame& frame, Ref<FrameBuffer> fb) = 0;

    };
}