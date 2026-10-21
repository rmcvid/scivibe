#pragma once
#include "core/core.hpp"
#include <vector>
#include <cstdint>
#include "pch/pch.hpp"
#include <glm/glm.hpp>
#include "objet/color.hpp"
#include "objet/line.hpp"

namespace scivibe {
    enum class CurveJoin {
        Miter,
        Bevel,
        Round
    };
    enum class CurveCap {
        Butt,
        Square,
        Round
    };
    struct CurveStyle {
        glm::vec4 Color{1.0f};
        float WidthPixels = 2.0f;
        CurveJoin Join = CurveJoin::Round;
        CurveCap Cap = CurveCap::Round;
        float MiterLimit = 4.0f;
    };
    struct CurveContour {
        std::vector<glm::dvec2> Points;
        bool Closed = false;
    };
    struct CurvePath {
        std::vector<CurveContour> Contours;
    };
    struct CurveMesh {
        std::vector<glm::vec3> Positions;
        std::vector<uint32_t> Indices;
    };

    class SCIVIBE_API RendererCurve2D {
        public:
            static void DrawLine(const glm::dvec2& start,const glm::dvec2& end,const CurveStyle& style,float z = 0.0f );
            static void DrawLine( const glm::dvec2& start, const glm::dvec2& end, const Color& color = Color::White, float thickness = s_CurveStyle.WidthPixels, float z = 0.0f);
            static void DrawLine(const Line& line);
            static void DrawPolyline(const std::vector<glm::dvec2>& points,const CurveStyle& style,bool closed = false,float z = 0.0f);
            static void DrawPath(const CurvePath& path,const CurveStyle& style,float z = 0.0f);
            static void DrawCarthesian(const glm::dvec3& center, const glm::dvec3 size, const glm::dvec3 maille);
            static void DrawCarthesian(const glm::dvec2& center, const glm::dvec2 size, const glm::dvec2 maille = {1,1}, const CurveStyle& style = s_CurveStyle, const glm::dvec2 depassement = {0,0});
            static void DrawCarthesian(const glm::dvec2& center, const glm::dvec2 size, const glm::dvec2 maille = {1,1}, const glm::dvec2 depassement = {0,0});
        private:
            friend class Renderer2D;
            static void SetView(const glm::mat4& viewProjection,const glm::uvec2& framebufferSize);
            static CurveMesh BuildStroke(const CurvePath& path,const CurveStyle& style,float z);
            static CurveStyle s_CurveStyle;
    };
}