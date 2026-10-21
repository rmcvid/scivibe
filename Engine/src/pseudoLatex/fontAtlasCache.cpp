#include "pseudoLatex/fontAtlasCache.hpp"
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <random>
#include <sstream>

namespace scivibe::fontcache {
    namespace {
        // Bump when glyph selection, packing, SDF settings or serialization changes.
        constexpr uint32_t Version = 1;
        constexpr uint32_t Magic = 0x53444643;
        constexpr std::size_t AlphaSize = std::size_t(AtlasWidth) * AtlasHeight;
        constexpr std::size_t MaxGlyphs = LastCodepoint - FirstCodepoint + 1;
        constexpr std::size_t RecordSize = 40;
        constexpr std::size_t HeaderSize = 24;
        constexpr uint64_t HashSeed = 14695981039346656037ull;

        uint64_t Hash(const uint8_t* data, std::size_t size, uint64_t hash = HashSeed) {
            for (std::size_t i = 0; i < size; ++i) {
                hash ^= data[i];
                hash *= 1099511628211ull;
            }
            return hash;
        }

        void Put32(std::vector<uint8_t>& bytes, uint32_t value) {
            for (unsigned shift = 0; shift < 32; shift += 8)
                bytes.push_back(static_cast<uint8_t>(value >> shift));
        }
        void Put64(std::vector<uint8_t>& bytes, uint64_t value) {
            Put32(bytes, static_cast<uint32_t>(value));
            Put32(bytes, static_cast<uint32_t>(value >> 32));
        }
        void PutFloat(std::vector<uint8_t>& bytes, float value) {
            uint32_t bits;
            static_assert(sizeof(value) == sizeof(bits));
            std::memcpy(&bits, &value, sizeof(bits));
            Put32(bytes, bits);
        }
        // Callers check the entire record length before reading it.
        uint32_t Get32(const uint8_t*& p) {
            uint32_t value = 0;
            for (unsigned shift = 0; shift < 32; shift += 8)
                value |= uint32_t(*p++) << shift;
            return value;
        }
        uint64_t Get64(const uint8_t*& p) {
            const uint64_t low = Get32(p);
            return low | (uint64_t(Get32(p)) << 32);
        }
        int32_t GetSigned(const uint8_t*& p) {
            const uint32_t value = Get32(p);
            return value <= INT32_MAX ? static_cast<int32_t>(value)
                : static_cast<int32_t>(int64_t(value) - 0x100000000ll);
        }
        float GetFloat(const uint8_t*& p) {
            const uint32_t bits = Get32(p);
            float value;
            std::memcpy(&value, &bits, sizeof(value));
            return value;
        }
    }

    std::optional<Key> MakeKey(const std::string& fontPath, uint32_t pixelSize,
                               const std::string& rasterizer) {
        std::ifstream font(fontPath, std::ios::binary);
        if (!font) return std::nullopt;
        uint64_t hash = HashSeed;
        std::array<uint8_t, 16384> buffer;
        while (font.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || font.gcount())
            hash = Hash(buffer.data(), static_cast<std::size_t>(font.gcount()), hash);
        if (!font.eof()) return std::nullopt;

        // Content-based: replacing a TTF invalidates its cache; identical files share it.
        const std::string settings = std::to_string(Version) + ":" +
            std::to_string(pixelSize) + ":" + std::to_string(AtlasWidth) + ":" +
            std::to_string(AtlasHeight) + ":" + std::to_string(Padding) + ":" +
            std::to_string(FirstCodepoint) + ":" + std::to_string(LastCodepoint) +
            ":" + rasterizer;
        hash = Hash(reinterpret_cast<const uint8_t*>(settings.data()), settings.size(), hash);
        if (const char* properties = std::getenv("FREETYPE_PROPERTIES"))
            hash = Hash(reinterpret_cast<const uint8_t*>(properties), std::strlen(properties), hash);
        std::ostringstream name;
        name << std::hex << std::setfill('0') << std::setw(16) << hash << ".sdfcache";
        return Key{hash, std::filesystem::path(SCIVIBE_FONT_CACHE_PATH) / name.str()};
    }

    bool Load(const Key& key, std::unordered_map<char32_t, Glyph>& glyphs,
              float& lineHeight, std::vector<uint8_t>& pixels) {
        std::ifstream file(key.File, std::ios::binary | std::ios::ate);
        if (!file) return false;
        const auto size = file.tellg();
        if (size < static_cast<std::streamoff>(HeaderSize + 8 + AlphaSize) ||
            size > static_cast<std::streamoff>(HeaderSize + 8 + MaxGlyphs * RecordSize + AlphaSize))
            return false;
        std::vector<uint8_t> bytes(static_cast<std::size_t>(size));
        file.seekg(0);
        if (!file.read(reinterpret_cast<char*>(bytes.data()), bytes.size())) return false;
        const uint8_t* p = bytes.data();
        if (Get32(p) != Magic || Get32(p) != Version || Get64(p) != key.Hash) return false;
        const auto checksum = Get64(p);
        if (checksum != Hash(p, bytes.size() - HeaderSize)) return false;
        const float height = GetFloat(p);
        const uint32_t count = Get32(p);
        if (!std::isfinite(height) || height <= 0 || count > MaxGlyphs ||
            bytes.size() != HeaderSize + 8 + count * RecordSize + AlphaSize) return false;

        std::unordered_map<char32_t, Glyph> loaded;
        for (uint32_t i = 0; i < count; ++i) {
            const char32_t codepoint = Get32(p);
            Glyph glyph;
            glyph.Size.x = GetSigned(p); glyph.Size.y = GetSigned(p);
            glyph.Bearing.x = GetSigned(p); glyph.Bearing.y = GetSigned(p);
            glyph.Advance = GetSigned(p);
            glyph.UVMin.x = GetFloat(p); glyph.UVMin.y = GetFloat(p);
            glyph.UVMax.x = GetFloat(p); glyph.UVMax.y = GetFloat(p);
            if (codepoint < FirstCodepoint || codepoint > LastCodepoint ||
                (codepoint >= 127 && codepoint < 160) ||
                glyph.Size.x < 0 || glyph.Size.x > static_cast<int>(AtlasWidth) ||
                glyph.Size.y < 0 || glyph.Size.y > static_cast<int>(AtlasHeight)) return false;
            for (int axis = 0; axis < 2; ++axis) {
                if (!std::isfinite(glyph.UVMin[axis]) || !std::isfinite(glyph.UVMax[axis]) ||
                    glyph.UVMin[axis] < 0 || glyph.UVMax[axis] > 1 ||
                    glyph.UVMin[axis] > glyph.UVMax[axis]) return false;
            }
            if (!loaded.emplace(codepoint, glyph).second) return false;
        }
        if (loaded.find(U'?') == loaded.end()) return false;

        // RGB is always white. Store only SDF alpha on disk, preserving orientation.
        std::vector<uint8_t> rgba(AlphaSize * 4, 255);
        for (std::size_t i = 0; i < AlphaSize; ++i) rgba[i * 4 + 3] = p[i];
        glyphs = std::move(loaded);
        lineHeight = height;
        pixels = std::move(rgba);
        return true;
    }

    bool Save(const Key& key, const std::unordered_map<char32_t, Glyph>& glyphs, float lineHeight, const std::vector<uint8_t>& pixels) {
        if (pixels.size() != AlphaSize * 4 || glyphs.size() > MaxGlyphs) return false;
        std::vector<uint8_t> payload;
        payload.reserve(8 + glyphs.size() * RecordSize + AlphaSize);
        PutFloat(payload, lineHeight);
        Put32(payload, static_cast<uint32_t>(glyphs.size()));
        // Stable ordering, no struct dumps (padding/GLM ABI are not serialized).
        for (char32_t codepoint = FirstCodepoint; codepoint <= LastCodepoint; ++codepoint) {
            const auto it = glyphs.find(codepoint);
            if (it == glyphs.end()) continue;
            const auto& glyph = it->second;
            Put32(payload, codepoint);
            Put32(payload, glyph.Size.x); Put32(payload, glyph.Size.y);
            Put32(payload, glyph.Bearing.x); Put32(payload, glyph.Bearing.y);
            Put32(payload, glyph.Advance);
            PutFloat(payload, glyph.UVMin.x); PutFloat(payload, glyph.UVMin.y);
            PutFloat(payload, glyph.UVMax.x); PutFloat(payload, glyph.UVMax.y);
        }
        for (std::size_t i = 3; i < pixels.size(); i += 4) payload.push_back(pixels[i]);
        std::vector<uint8_t> header;
        Put32(header, Magic); Put32(header, Version);
        Put64(header, key.Hash); Put64(header, Hash(payload.data(), payload.size()));

        std::error_code error;
        std::filesystem::create_directories(key.File.parent_path(), error);
        if (error) return false;
        auto temporary = key.File;
        // Separate writers never publish a partially written atlas.
        temporary += "." + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
            "." + std::to_string(std::random_device{}()) + ".tmp";
        {
            std::ofstream file(temporary, std::ios::binary | std::ios::trunc);
            file.write(reinterpret_cast<const char*>(header.data()), header.size());
            file.write(reinterpret_cast<const char*>(payload.data()), payload.size());
            file.close();
            if (!file) {
                std::filesystem::remove(temporary, error);
                return false;
            }
        }
        std::filesystem::rename(temporary, key.File, error);
        std::error_code typeError;
        if (error && std::filesystem::is_regular_file(key.File, typeError)) {
            // Windows rename cannot replace an existing (e.g. corrupt) cache file.
            std::filesystem::remove(key.File, error);
            if (!error) std::filesystem::rename(temporary, key.File, error);
        }
        if (!error) return true;
        std::filesystem::remove(temporary, error);
        return false;
    }
}
