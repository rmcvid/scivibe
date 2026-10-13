#pragma once 
#include "core/core.hpp"
#include "glm/glm.hpp"
#include <string>
#include <string_view>
#include <memory>
#include <cstdint>

namespace tex{
    class TeXRender;
    class Graphics2D;
}

namespace scivibe{
    struct LatexSettings {
        std::string ResourceDirectory;
        int MaxWidth = 800;
        float TextSize = 48.0f;
        float LineSpacing = 10.0f;
        uint32_t RasterSize = 48;
        float UnitsPerPixel = 0.005f;
    };

    class SCIVIBE_API LatexFormula{
        public:
            LatexFormula();
            ~LatexFormula();
            LatexFormula(const LatexFormula&) = delete;
            LatexFormula& operator=(const LatexFormula& ) = delete;
        private:
            friend class PseudoLatex;
            Scope<tex::TeXRender> m_TexRender;
    };
    class SCIVIBE_API PseudoLatex{
        public:
            static void Init(const LatexSettings& settings = {});
            static void Shutdown();
            static Ref<LatexFormula> Create(std::string_view source);
            static void Write(const LatexFormula& formula,const glm::vec3& position);
            static void Write(std::string_view source,const glm::vec3& position);
      
    };

}
