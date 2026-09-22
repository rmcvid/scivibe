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
}