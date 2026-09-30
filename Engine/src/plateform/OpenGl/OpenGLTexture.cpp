#include "plateform/OpenGl/OpenGLTexture.hpp"
#include <glad/glad.h>
#include "stb_image.h"
#include "log/log.hpp"


namespace scivibe{
    OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
    : m_Path(path){
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        SCIVIBE_ASSERT(data, "failed to load image");
        m_Width = width;
        m_Height = height;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // pourquoi comme ca ?
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,m_Width,m_Height,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
        glTexSubImage2D(GL_TEXTURE_2D,0,0, 0,m_Width, m_Height,GL_RGBA,GL_UNSIGNED_BYTE,data);
        stbi_image_free(data);

    }
    OpenGLTexture2D::~OpenGLTexture2D(){
        glDeleteTextures(1,&m_RendererID);

    }
    void OpenGLTexture2D::Bind(uint32_t slot) const {
        glBindTexture(slot,m_RendererID);
    }
    
}