#include "pseudoLatex/pseudoLatex.hpp"
#include "renderer/renderer2D.hpp"
namespace scivibe{
    void PseudoLatex::Write(std::u32string_view text,Font& font,const glm::vec3& position,float scale,const glm::vec4& color){
        auto parsedText = Parse(text);
        Renderer2D::Write(parsedText,font,position,scale,color);
    }
    std::u32string PseudoLatex::Parse(std::u32string_view text){
        return static_cast<std::u32string>(text);;
    }
}