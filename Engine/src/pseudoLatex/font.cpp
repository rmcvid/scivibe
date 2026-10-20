#include "pseudoLatex/font.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <algorithm>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace scivibe{
    namespace {
        void CheckFreeType(FT_Error error, const char* operation) {
            if (error != 0) {
                throw std::runtime_error(std::string(operation)+ " : erreur FreeType "+ std::to_string(error));
            }
        }
    }
    Font::Font(const std::string& filepath, uint32_t pixelSize){
        if(pixelSize ==0 ){
            throw std::invalid_argument("La taille de la police doit etre superieure a zero");
        }
        FT_Library rawLibrary = nullptr;
        CheckFreeType(FT_Init_FreeType(&rawLibrary),"FT_Init_FreeType");
        std::unique_ptr<std::remove_pointer_t<FT_Library>, decltype(&FT_Done_FreeType)> library(rawLibrary, &FT_Done_FreeType);
        FT_Face rawFace = nullptr;
        CheckFreeType(FT_New_Face(library.get(), filepath.c_str(), 0, &rawFace),"FT_New_Face");
        std::unique_ptr<std::remove_pointer_t<FT_Face>,decltype(&FT_Done_Face)> face(rawFace, &FT_Done_Face);
        CheckFreeType(FT_Select_Charmap(face.get(), FT_ENCODING_UNICODE),"FT_Select_Charmap");
        CheckFreeType(FT_Set_Pixel_Sizes(face.get(), 0, pixelSize),"FT_Set_Pixel_Sizes");
        // ici à vérifier
        m_LineHeight = static_cast<float>(face->size->metrics.height) / 64.0f;

        constexpr uint32_t atlasWidth = 2048;
        constexpr uint32_t atlasHeight = 2048;
        constexpr uint32_t padding = 1;

        std::vector<uint8_t> pixels(static_cast<std::size_t>(atlasWidth) * atlasHeight * 4,255);
        for (std::size_t i = 3; i < pixels.size(); i += 4) {pixels[i] = 0;}

        uint32_t cursorX = padding;
        uint32_t cursorY = padding;
        uint32_t rowHeight = 0;

        for(char32_t codepoint = 32; codepoint <= 255; ++codepoint){
            if(codepoint >=127 && codepoint <160) continue;
            if(FT_Get_Char_Index(face.get(), codepoint) == 0) continue;
            CheckFreeType(FT_Load_Char(face.get(),codepoint,FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING),"FT_Load_Char");
            CheckFreeType(FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF),"FT_Render_Glyph SDF");
            const FT_GlyphSlot slot = face->glyph;
            const FT_Bitmap& bitmap = slot->bitmap;

            const uint32_t width = bitmap.width;
            const uint32_t height = bitmap.rows;

            Glyph glyph{};
            glyph.Size = {
                static_cast<int>(width),
                static_cast<int>(height)
            };
            glyph.Bearing = {
                slot->bitmap_left,
                slot->bitmap_top
            };
            glyph.Advance = static_cast<int32_t>(slot->advance.x);
            if (width > 0 && height > 0) {
                if (bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
                    throw std::runtime_error(
                        "Le rendu SDF attend un bitmap mono-canal 8 bits"
                    );
                }
                if (width + 2 * padding > atlasWidth
                    || height + 2 * padding > atlasHeight) {
                    throw std::runtime_error(
                        "Un glyphe est trop grand pour l'atlas"
                    );
                }

                // Passer à la rangée suivante si nécessaire.
                if (cursorX + width + padding > atlasWidth) {
                    cursorX = padding;
                    cursorY += rowHeight + padding;
                    rowHeight = 0;
                }

                if (cursorY + height + padding > atlasHeight) {
                    throw std::runtime_error(
                        "Atlas plein : reduire pixelSize ou agrandir l'atlas"
                    );
                }

                // Copier le bitmap dans le canal alpha.
                for (uint32_t row = 0; row < height; ++row) {
                    const auto* sourceRow = bitmap.buffer + static_cast<std::ptrdiff_t>(row) * bitmap.pitch;

                    const uint32_t destinationY = cursorY + height - 1 - row;
                    for (uint32_t column = 0; column < width; ++column) {
                        const std::size_t destination =
                            (   static_cast<std::size_t>(destinationY) * atlasWidth 
                            +   cursorX + column ) * 4;
                        pixels[destination + 3] = sourceRow[column];
                    }
                }

                glyph.UVMin = {
                    static_cast<float>(cursorX) / atlasWidth,
                    static_cast<float>(cursorY) / atlasHeight
                };

                glyph.UVMax = {
                    static_cast<float>(cursorX + width) / atlasWidth,
                    static_cast<float>(cursorY + height) / atlasHeight
                };

                cursorX += width + padding;
                rowHeight = std::max(rowHeight, height);
            }

            m_GlyphCache.emplace(codepoint, glyph);
        }

        if (m_GlyphCache.find(U'?') == m_GlyphCache.end()) 
            throw std::runtime_error( "La police ne contient pas le caractere de remplacement '?'" );
        m_AtlasTexture = Texture2D::Create(atlasWidth, atlasHeight);
        if (!m_AtlasTexture) throw std::runtime_error( "Impossible de creer la texture de la police" );
        m_AtlasTexture->SetData( pixels.data(),static_cast<uint32_t>(pixels.size()) );
    }
    Font::~Font() = default;

    const Glyph& Font::GetGlyph(char32_t codepoint) const {
        auto it = m_GlyphCache.find(codepoint);
        if (it != m_GlyphCache.end())
            return it->second;

        return m_GlyphCache.at(U'?');
    }
}
