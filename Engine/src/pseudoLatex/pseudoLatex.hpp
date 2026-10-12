#pragma once 
#include "pseudoLatex/font.hpp"
#include "glm/glm.hpp"
#include <string>
#include <string_view>
namespace scivibe{
    class PseudoLatex{
        public:
            static void Write(std::u32string_view text,Font& font,const glm::vec3& position,float scale,const glm::vec4& color);
            static void SetFont();
        private:
            static std::u32string Parse(std::u32string_view text);
            static Font& s_Font;
    };

}
