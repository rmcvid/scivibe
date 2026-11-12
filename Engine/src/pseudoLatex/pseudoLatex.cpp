#include "pseudoLatex/pseudoLatex.hpp"
#include "pseudoLatex/microtexAdapter.hpp"
#include <glm/common.hpp>
#include <latex.h>
#include <utils/utf.h>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <tuple>
namespace scivibe{
    namespace {
        enum class LatexState {
            NotInitialized,
            Ready,
            Stopped
        };
        LatexState s_State = LatexState::NotInitialized;
        LatexSettings s_Settings;

        using FormulaKey = std::tuple<std::string, float, int, float>;
        std::map<FormulaKey, Ref<LatexFormula>> s_FormulaCache;
    }
    LatexFormula::LatexFormula() = default;
    LatexFormula::~LatexFormula() = default;

    void PseudoLatex::Init(const LatexSettings& settings){
        s_Settings = settings;
        ConfigureMicroTeXAdapter(settings.RasterSize);
        const std::string path = settings.ResourceDirectory.empty()
            ? SCIVIBE_MICROTEX_RESOURCE_PATH
            : settings.ResourceDirectory;
        tex::LaTeX::init(path);
        tex::LaTeX::setDebug(false);
        PreloadMicroTeXFonts();
        s_State = LatexState::Ready;
    }
    
    // ici formule à modifier
    Ref<LatexFormula> PseudoLatex::Create(std::string_view source){
        auto formula = CreateRef<LatexFormula>();
        formula->m_TexRender.reset( tex::LaTeX::parse(tex::utf82wide(std::string(source)),
            s_Settings.MaxWidth,s_Settings.TextSize,s_Settings.LineSpacing,tex::white));
        return formula;
    }
    Ref<LatexFormula> PseudoLatex::Create(std::string_view source,float fontSize, int maxWidth, float lineSpacing){
        auto formula = CreateRef<LatexFormula>();
        formula->m_TexRender.reset( tex::LaTeX::parse(tex::utf82wide(std::string(source)),maxWidth,fontSize,lineSpacing,tex::white));
        return formula;
    }
    void PseudoLatex::Write(std::string_view source, const glm::vec3& position,float fontSize, int maxWidth, float lineSpacing) {
        Write(source, glm::translate(glm::mat4(1.0f), position), fontSize, maxWidth, lineSpacing);
    }
    void PseudoLatex::Write(std::string_view source, const glm::mat4& transform,float fontSize, int maxWidth, float lineSpacing, const glm::vec4& tint) {
        if (s_State != LatexState::Ready) throw std::logic_error("Appeler PseudoLatex::Init avant Write");
        const FormulaKey key{std::string(source), fontSize, maxWidth, lineSpacing};
        auto it = s_FormulaCache.find(key);
        if (it == s_FormulaCache.end()) {
            auto formula = Create(source, fontSize, maxWidth, lineSpacing);
            if (s_FormulaCache.size() >= 128) s_FormulaCache.clear();
            it = s_FormulaCache.emplace(key, std::move(formula)).first;
        }
        if (it->second->m_TexRender)
            DrawMicroTeX(*it->second->m_TexRender, transform, s_Settings.UnitsPerPixel, tint);
    }

    void PseudoLatex::Write(const LatexFormula& formula,const glm::vec3& position) {
        if (s_State != LatexState::Ready || !formula.m_TexRender)
            throw std::logic_error("MicroTeX non initialise ou formule absente");
        DrawMicroTeX(*formula.m_TexRender,position,s_Settings.UnitsPerPixel);
    }

    void PseudoLatex::Write(std::string_view source,const glm::vec3& position) {
        Write(source, position, s_Settings.TextSize, s_Settings.MaxWidth, s_Settings.LineSpacing);
    }

    void PseudoLatex::WriteFromFile(std::string filePath, const glm::vec3& position){
        
    }
    void PseudoLatex::Shutdown() {
        if (s_State != LatexState::Ready) { return;}
        s_FormulaCache.clear();
        tex::LaTeX::release();
        s_State = LatexState::Stopped;
    }
}
