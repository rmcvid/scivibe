#pragma once
#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace tex{
    class TeXRender;
}
namespace scivibe {
    void ConfigureMicroTeXAdapter(uint32_t rasterSize);
    void DrawMicroTeX(tex::TeXRender& formula,const glm::vec3& position,float unitsPerPixel);
    void DrawMicroTeX(tex::TeXRender& formula,const glm::mat4& transform,float unitsPerPixel,
        const glm::vec4& tint = glm::vec4(1.0f));
    void PreloadMicroTeXFonts();
}
