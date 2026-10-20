#pragma once
#include "core/core.hpp"
#include "pch/pch.hpp"
#include "core/log.hpp"
#include <filesystem>

namespace scivibe{
    class SCIVIBE_API PsFile{
        public:
            PsFile() = default;
            ~PsFile() = default;
            bool Load(const std::string& filePath);
            const std::string& GetParagraph() const;
            const std::string& GetParagraph(size_t index) const;
            size_t GetParagraphCount() const;
            size_t GetParagraphIndex() const;
            bool HasChanged(const std::string& filePath);
            bool Update(const std::string& filePath);
            void NextParagraph();
            void PreviousParagraph();
            void Reset();

        private:
            uint32_t m_Index {0};
            bool m_HasChanged = false;
            std::filesystem::file_time_type m_LastSaved {};
            std::vector<std::string> m_Paragraphs;
    };
}
