#pragma once
#include "renderer/graphicsContext.hpp"
#include "pch/pch.hpp"
#include "glfw/glfw3.h"
#include "glad/glad.h"
#include "renderer/frameBuffer.hpp"

namespace scivibe {
    class OpenGLContext : public GraphicsContext{
        public :
            OpenGLContext(GLFWwindow* windowHandle);
            void Init() override;
            void SwapBuffers() override;
            bool CaptureFrame(CapturedFrame& frame) override;
            bool CaptureFrame(CapturedFrame& frame, Ref<FrameBuffer> fb) override;


        private:
            GLFWwindow* m_windowHandle;
    };

}