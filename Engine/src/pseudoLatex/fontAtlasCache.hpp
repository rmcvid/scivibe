#pragma once

#include "pseudoLatex/font.hpp"
#include <filesystem>
#include <optional>
#include <vector>

// Internal CPU-side cache: no OpenGL calls and no DLL API.
namespace scivibe::fontcache {
    inline constexpr uint32_t AtlasWidth = 2048;
    inline constexpr uint32_t AtlasHeight = 2048;
    inline constexpr uint32_t Padding = 1;
    inline constexpr char32_t FirstCodepoint = 32;
    inline constexpr char32_t LastCodepoint = 255;

    struct Key {uint64_t Hash;std::filesystem::path File;};
    std::optional<Key> MakeKey(const std::string& fontPath, uint32_t pixelSize,const std::string& rasterizer);
    bool Load(const Key& key, std::unordered_map<char32_t, Glyph>& glyphs,float& lineHeight, std::vector<uint8_t>& pixels);
    bool Save(const Key& key, const std::unordered_map<char32_t, Glyph>& glyphs,float lineHeight, const std::vector<uint8_t>& pixels);
}
