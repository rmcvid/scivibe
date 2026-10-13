#include "pseudoLatex/pseudoLatex.hpp"
#include "pseudoLatex/microtexAdapter.hpp"
#include <glm/common.hpp>
#include <latex.h>
#include <utils/utf.h>
#include <stdexcept>
namespace scivibe{
    namespace {
        enum class LatexState {
            NotInitialized,
            Ready,
            Stopped
        };
        LatexState s_State = LatexState::NotInitialized;
        LatexSettings s_Settings;

        std::unordered_map<std::string,Ref<LatexFormula>> s_FormulaCache;
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
        s_State = LatexState::Ready;
}
    
    // ici formule à modifier
    Ref<LatexFormula> PseudoLatex::Create(std::string_view source){
        auto formula = CreateRef<LatexFormula>();
        formula->m_TexRender.reset( tex::LaTeX::parse(tex::utf82wide(std::string(source)),
            s_Settings.MaxWidth,s_Settings.TextSize,s_Settings.LineSpacing,tex::white));
        return formula;
    }
    void PseudoLatex::Write(const LatexFormula& formula,const glm::vec3& position) {
        if (s_State != LatexState::Ready || !formula.m_TexRender)
            throw std::logic_error("MicroTeX non initialise ou formule absente");
        DrawMicroTeX(*formula.m_TexRender,position,s_Settings.UnitsPerPixel);
        }

        void PseudoLatex::Write(std::string_view source,const glm::vec3& position) {
        if (s_State != LatexState::Ready) throw std::logic_error("Appeler PseudoLatex::Init avant Write");
        const std::string key(source);
        auto it = s_FormulaCache.find(key);
        if (it == s_FormulaCache.end()) {
            auto formula = Create(source);
            if (s_FormulaCache.size() >= 128) s_FormulaCache.clear();
            it = s_FormulaCache.emplace(key, formula).first;
        }
        Write(*it->second, position);
    }
    void PseudoLatex::Shutdown() {
        if (s_State != LatexState::Ready) { return;}
        s_FormulaCache.clear();
        tex::LaTeX::release();
        s_State = LatexState::Stopped;
    }
}
