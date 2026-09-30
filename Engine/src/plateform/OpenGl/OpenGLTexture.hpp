#pragma once
#include <string>
#include "pch/pch.hpp"
#include "renderer/texture.hpp"
#include <glad/glad.h>


namespace scivibe{
    class OpenGLTexture2D : public Texture2D{
        public:
            OpenGLTexture2D(const std::string& path);
            virtual ~OpenGLTexture2D();
            virtual uint32_t GetWidth() const  override {return m_Width;};
            virtual uint32_t GetHeight() const override {return m_Height;};
            virtual void Bind(uint32_t slot) const override;

        private:
            const std::string& m_Path;
            uint32_t m_Width;
            uint32_t m_Height;
            uint32_t m_RendererID;
            void GetFormat(int channels, GLenum& internalForma, GLenum& dataformat );
    };

}