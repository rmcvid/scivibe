#pragma once

#include "core/core.hpp"
#include "renderer/texture.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>

namespace scivibe {

    struct Glyph {
        glm::ivec2 Size{0};
        glm::ivec2 Bearing{0};

        // Valeur FreeType en 1/64 de pixel.
        int32_t Advance = 0;

        glm::vec2 UVMin{0.0f};
        glm::vec2 UVMax{0.0f};
    };

    class SCIVIBE_API Font {
    public:
        Font(const std::string& filepath, uint32_t pixelSize);
        ~Font();

        const Glyph& GetGlyph(char32_t codepoint) const;

        const Ref<Texture2D>& GetAtlasTexture() const {
            return m_AtlasTexture;
        }

        float GetLineHeight() const {return m_LineHeight;};

    private:
        Ref<Texture2D> m_AtlasTexture;
        std::unordered_map<char32_t, Glyph> m_GlyphCache;
        float m_LineHeight = 0.0f;
    };

}