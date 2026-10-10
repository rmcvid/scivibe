#include "plateform/OpenGl/OpenGLContext.hpp"
//#include "GL/gl.h"

namespace scivibe{
    
    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_windowHandle(windowHandle){
            SCIVIBE_CORE_ASSERT(windowHandle, "window handle is null");
    }
    void OpenGLContext::Init(){
        glfwMakeContextCurrent(m_windowHandle);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            SCIVIBE_CORE_ERROR("Impossible d'initialiser GLAD");
            glfwDestroyWindow(m_windowHandle);
            m_windowHandle = nullptr;
            throw std::runtime_error("gladLoadGLLoader failed");
        }
        SCIVIBE_CORE_INFO("OpenGL Info");
        SCIVIBE_CORE_INFO("vendor : {0}", glGetString(GL_VENDOR));
        SCIVIBE_CORE_INFO("Renderrer : {0}", glGetString(GL_RENDERER));
        SCIVIBE_CORE_INFO("Version : {0}", glGetString(GL_VERSION));
    }
    
    void OpenGLContext::SwapBuffers(){
        glfwSwapBuffers(m_windowHandle);   
    }
    bool OpenGLContext::CaptureFrame(CapturedFrame& frame){
    glfwGetFramebufferSize(m_windowHandle,&frame.Width,&frame.Height);
    if (frame.Width <= 0 || frame.Height <= 0)
        return false;
    constexpr int channels = 4;
    frame.StrideBytes = frame.Width * channels;
    frame.Pixels.resize(static_cast<size_t>(frame.StrideBytes) *static_cast<size_t>(frame.Height));
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0,0,frame.Width,frame.Height,GL_RGBA,GL_UNSIGNED_BYTE,frame.Pixels.data());
    GLenum error = glGetError();
    if (error != GL_NO_ERROR){
        SCIVIBE_CORE_ERROR("OpenGL CaptureFrame failed: {0}",error);
        return false;
    }
    return true;
}
}