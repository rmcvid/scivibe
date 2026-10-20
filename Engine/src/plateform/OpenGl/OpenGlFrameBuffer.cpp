#include "plateform/OpenGl/OpenGlFrameBuffer.hpp"
#include "glad/glad.h"
#include "glfw/glfw3.h"

namespace scivibe{
    OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
        : m_Spec(spec)
    {
        Invalidate();
    }
    OpenGLFrameBuffer::~OpenGLFrameBuffer(){
        glDeleteFramebuffers(1,&m_RendererID);
        glDeleteTextures(1, &m_ColorAttachment);
        glDeleteTextures(1, &m_DepthAttachment);
    }
    /*ancienne version*/
#if 0 
    void OpenGLFrameBuffer::Invalidate(){
        if(m_RendererID){
            glDeleteFramebuffers(1,&m_RendererID);
            glDeleteTextures(1, &m_ColorAttachment);
            glDeleteTextures(1, &m_DepthAttachment);
        }
        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,m_Spec.Width,m_Spec.Height,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,m_ColorAttachment,0);
        glGenTextures(1, &m_DepthAttachment);
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH24_STENCIL8,m_Spec.Width,m_Spec.Height,0,GL_DEPTH_STENCIL,GL_UNSIGNED_INT_24_8,nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_TEXTURE_2D,m_DepthAttachment,0);
        SCIVIBE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,"Framebuffer incomplet");
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
#endif
    void OpenGLFrameBuffer::Invalidate() {
    glDeleteFramebuffers(1, &m_RendererID);
    glDeleteTextures(1, &m_ColorAttachment);
    glDeleteTextures(1, &m_DepthAttachment);
    glDeleteFramebuffers(1, &m_ResolveFramebuffer);
    glDeleteTextures(1, &m_ResolveColorAttachment);
    constexpr GLsizei samples = 4;
    const GLsizei width = static_cast<GLsizei>(m_Spec.Width);
    const GLsizei height = static_cast<GLsizei>(m_Spec.Height);
    glGenFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    glGenTextures(1, &m_ColorAttachment);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples,GL_RGBA8, width, height, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment, 0);
    glGenTextures(1, &m_DepthAttachment);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples,GL_DEPTH24_STENCIL8, width, height, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment, 0);
    SCIVIBE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,"Framebuffer MSAA incomplet");
    glGenFramebuffers(1, &m_ResolveFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ResolveFramebuffer);
    glGenTextures(1, &m_ResolveColorAttachment);
    glBindTexture(GL_TEXTURE_2D, m_ResolveColorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_ResolveColorAttachment, 0);
    SCIVIBE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,"Framebuffer de resolution incomplet");
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
    void OpenGLFrameBuffer::Bind(){
        glBindFramebuffer(GL_FRAMEBUFFER,m_RendererID);
    }
#if 0
    void OpenGLFrameBuffer::Unbind(){
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }
#endif
    void OpenGLFrameBuffer::Unbind() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ResolveFramebuffer);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, m_Spec.Width, m_Spec.Height,0, 0, m_Spec.Width, m_Spec.Height,GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
    void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height){
        m_Spec.Width = width;
        m_Spec.Height = height;
        Invalidate();
    }
    void OpenGLFrameBuffer::BindRead() {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ResolveFramebuffer);
    }
}
