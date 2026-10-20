#pragma once
#include <cstdint>
#include <glm/vec3.hpp>

namespace tex{
    class TeXRender;
}
namespace scivibe {
    void ConfigureMicroTeXAdapter(uint32_t rasterSize);
    void DrawMicroTeX(tex::TeXRender& formula,const glm::vec3& position,float unitsPerPixel);
    void PreloadMicroTeXFonts();
}