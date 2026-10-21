#pragma once
#include "pch/pch.hpp"
#include <glm/glm.hpp>
namespace scivibe{
    class SCIVIBE_API Texture{
        public:
            virtual ~Texture() = default;
            virtual uint32_t GetWidth() const =0;
            virtual uint32_t GetHeight() const =0;
            virtual uint32_t GetRendererID() const =0;
            virtual void SetData(void* data, uint32_t size) = 0;

            virtual void Bind(uint32_t slot) const =0;
            virtual bool operator==(const Texture& other) const = 0;
    };

    
    class SCIVIBE_API Texture2D : public Texture{
        public:
        static Ref<Texture2D> Create(uint32_t width, uint32_t height);          
        static Ref<Texture2D> Create(const std::string& path);
    };
    class SCIVIBE_API SubTexture2D{
        public:
            SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);
            const Ref<Texture2D> GetTexture() const {return m_Texture;};
            const glm::vec2* GettexCoords()const {return m_TexCoords;};
            static Ref<SubTexture2D> createFromCoords(const Ref<Texture2D>& texture, glm::vec2& coords,  const glm::vec2& cellSize, const glm::vec2& spriteSize = {1.0f,1.0f});
            const glm::vec2& GetUVMin() const {return m_TexCoords[0];};
            const glm::vec2& GetUVMax() const {return m_TexCoords[2];};
            
        private:
            Ref<Texture2D> m_Texture;
            glm::vec2 m_TexCoords[4];
    };
}
