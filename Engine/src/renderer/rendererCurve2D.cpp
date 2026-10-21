#include "renderer/rendererCurve2D.hpp"
#include "renderer/renderer2D.hpp"
#include "pch/pch.hpp"

namespace scivibe{
    CurveStyle RendererCurve2D::s_CurveStyle{};
    namespace {
        glm::dmat4 s_CurveView{1.0};
        glm::uvec2 s_CurveViewport{0};
        bool s_CurveViewReady = false;
        constexpr double PointEpsilon = 1e-6; // Pixels du framebuffer.
        double Cross(const glm::dvec2& a, const glm::dvec2& b) { return a.x * b.y - a.y * b.x; }
        glm::dvec2 Normal(const glm::dvec2& direction) { return {-direction.y, direction.x}; }
    }
    void RendererCurve2D::DrawLine(const glm::dvec2& start,const glm::dvec2& end,const Color& color,float thickness,float z){
        CurveStyle style = s_CurveStyle;
        style.WidthPixels = thickness;
        style.Color = color.ToVec4();
        DrawPolyline({start, end}, style, false, z);
    }

    void RendererCurve2D::DrawLine(const glm::dvec2& start,const glm::dvec2& end,const CurveStyle& style,float z){
        DrawPolyline({start, end}, style, false, z);
    }

    void RendererCurve2D::DrawLine(const Line &line){
        DrawLine(line.m_Start,line.m_End,line.m_Color,line.m_Thickness,line.m_Position.z);
    }

    void RendererCurve2D::DrawPolyline(const std::vector<glm::dvec2>& points,const CurveStyle& style,bool closed,float z){
        CurvePath path;
        path.Contours.push_back(CurveContour{points, closed});
        DrawPath(path, style, z);
    }
    void RendererCurve2D::DrawPath(const CurvePath& path,const CurveStyle& style,float z){
        const CurveMesh mesh = BuildStroke(path, style, z);
        for (std::size_t i = 0; i < mesh.Indices.size(); i += 3){
            const auto& a = mesh.Positions[mesh.Indices[i]];
            const auto& b = mesh.Positions[mesh.Indices[i + 1]];
            const auto& c = mesh.Positions[mesh.Indices[i + 2]];
            Renderer2D::DrawTriangle(a, b, c, style.Color);
        }
    }
    
    void RendererCurve2D::SetView(const glm::mat4& viewProjection, const glm::uvec2& framebufferSize){
        s_CurveViewReady = false;
        for (int c = 0; c < 4; ++c)
            for (int r = 0; r < 4; ++r)
                if (!std::isfinite(viewProjection[c][r]))
                    throw std::invalid_argument("Matrice de courbe non finie");
        if (std::abs(viewProjection[0][3]) > 1e-8 ||
            std::abs(viewProjection[1][3]) > 1e-8 ||
            std::abs(viewProjection[2][3]) > 1e-8 ||
            std::abs(viewProjection[3][3]) < 1e-8)
            throw std::invalid_argument("BuildStroke exige une vue orthographique");
        s_CurveView = glm::dmat4(viewProjection);
        s_CurveViewport = framebufferSize;
        s_CurveViewReady = true;
    }
    CurveMesh RendererCurve2D::BuildStroke( const CurvePath& path, const CurveStyle& style, float z){
        CurveMesh mesh;
        if (!s_CurveViewReady) throw std::logic_error("Appeler SetView avant BuildStroke");
        if (!std::isfinite(style.WidthPixels) || !std::isfinite(z) || !std::isfinite(style.MiterLimit) || style.MiterLimit < 1.0f)
            throw std::invalid_argument("Style de courbe invalide");
        if (style.WidthPixels <= 0.0f || s_CurveViewport.x == 0 || s_CurveViewport.y == 0) return mesh;
        const double halfWidth = style.WidthPixels * 0.5;
        const glm::dvec2 viewport(s_CurveViewport);
        const double w = s_CurveView[3][3];
        const glm::dvec4 origin = s_CurveView * glm::dvec4(0, 0, z, 1);
        const glm::dmat3 toPixels(
            glm::dvec3(s_CurveView[0][0] * viewport.x / (2 * w), s_CurveView[0][1] * viewport.y / (2 * w), 0),
            glm::dvec3(s_CurveView[1][0] * viewport.x / (2 * w), s_CurveView[1][1] * viewport.y / (2 * w), 0),
            glm::dvec3((origin.x / w + 1) * viewport.x * 0.5, (origin.y / w + 1) * viewport.y * 0.5, 1));
        const double determinant = glm::determinant(toPixels);
        if (!std::isfinite(determinant) || determinant == 0.0) throw std::invalid_argument("Projection du plan de courbe non inversible");
        const glm::dmat3 toWorld = glm::inverse(toPixels);

        auto vertex = [&](const glm::dvec2& p) {
            const glm::dvec3 world = toWorld * glm::dvec3(p, 1);
            const double maxFloat = (std::numeric_limits<float>::max)();
            if (!std::isfinite(world.x) || !std::isfinite(world.y) || std::abs(world.x) > maxFloat || std::abs(world.y) > maxFloat)
                throw std::overflow_error("Sommet de courbe hors limites");
            return glm::vec3(static_cast<float>(world.x), static_cast<float>(world.y), z); };
        auto triangle = [&](glm::dvec2 a, glm::dvec2 b, glm::dvec2 c) {
            const double area = Cross(b - a, c - a);
            if (std::abs(area) < 1e-12) return;
            if (area < 0) std::swap(b, c); // Sens antihoraire a l'ecran.
            if (mesh.Positions.size() > (std::numeric_limits<uint32_t>::max)() - 3u)
                throw std::overflow_error("Maillage de courbe trop grand");
            const auto base = static_cast<uint32_t>(mesh.Positions.size());
            mesh.Positions.push_back(vertex(a));
            mesh.Positions.push_back(vertex(b));
            mesh.Positions.push_back(vertex(c));
            mesh.Indices.insert(mesh.Indices.end(), {base, base + 1, base + 2});
        };
        const double maxArcStep = std::max(1e-3, std::min( constants::Pi / 8.0, 2.0 * std::acos(std::clamp(1.0 - 0.25 / halfWidth, -1.0, 1.0))));
        auto arc = [&](const glm::dvec2& center, const glm::dvec2& pivot,
                    const glm::dvec2& first, const glm::dvec2& last,
                    double sweep) {
            const int steps = std::max(1, static_cast<int>(
                std::ceil(std::abs(sweep) / maxArcStep)));
            const double start = std::atan2(first.y - center.y, first.x - center.x);
            glm::dvec2 previous = first;
            for (int k = 1; k <= steps; ++k) {
                const double angle = start + sweep * k / steps;
                const glm::dvec2 next = k == steps ? last :
                    center + halfWidth * glm::dvec2(std::cos(angle), std::sin(angle));
                triangle(pivot, previous, next);
                previous = next;
            }
        };
        struct Segment {
            glm::dvec2 Direction;
            double Length;
            glm::dvec2 StartLeft, StartRight, EndLeft, EndRight;
        };
        for (const CurveContour& contour : path.Contours) {
            std::vector<glm::dvec2> points;
            for (const glm::dvec2& p : contour.Points) {
                if (!std::isfinite(p.x) || !std::isfinite(p.y))
                    throw std::invalid_argument(
                        "Point non fini : separer les branches avant BuildStroke");
                const glm::dvec2 screen(toPixels * glm::dvec3(p, 1));
                if (!std::isfinite(screen.x) || !std::isfinite(screen.y))
                    throw std::overflow_error("Projection de courbe hors limites");
                if (points.empty() || glm::length(screen - points.back()) > PointEpsilon)
                    points.push_back(screen);
            }
            if (contour.Closed && points.size() > 1 &&
                glm::length(points.front() - points.back()) <= PointEpsilon)
                points.pop_back();
            if (points.size() < (contour.Closed ? 3u : 2u)) continue;
            const size_t count = contour.Closed ? points.size() : points.size() - 1;
            std::vector<Segment> segments;
            segments.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                const glm::dvec2 a = points[i], b = points[(i + 1) % points.size()];
                const glm::dvec2 delta = b - a;
                const double length = glm::length(delta);
                if (!std::isfinite(length)) throw std::overflow_error("Segment de courbe hors limites");
                const glm::dvec2 direction = delta / length;
                const glm::dvec2 offset = Normal(direction) * halfWidth;
                segments.push_back({direction, length,a + offset, a - offset, b + offset, b - offset});}

            const size_t firstJoin = contour.Closed ? 0 : 1;
            const size_t endJoin = contour.Closed ? points.size() : points.size() - 1;
            for (size_t i = firstJoin; i < endJoin; ++i) {
                Segment& before = segments[(i + count - 1) % count];
                Segment& after = segments[i % count];
                const glm::dvec2 p = points[i];
                const glm::dvec2 n0 = Normal(before.Direction), n1 = Normal(after.Direction);
                const double turn = Cross(before.Direction, after.Direction);
                const double cosine = std::clamp(glm::dot(before.Direction, after.Direction), -1.0, 1.0);

                if (1.0 + cosine < 1e-8) {
                    if (style.Join == CurveJoin::Round)
                        arc(p, p, before.EndLeft, before.EndRight, -constants::Pi);
                    continue;
                }
                const glm::dvec2 miter = (n0 + n1) * (halfWidth / (1.0 + cosine));
                if (std::abs(turn) < 1e-8) {
                    before.EndLeft = after.StartLeft = p + miter;
                    before.EndRight = after.StartRight = p - miter;
                    continue;
                }

                const double side = turn > 0 ? 1.0 : -1.0;
                glm::dvec2 inner = p + side * miter;
                const bool tight =
                    std::abs(glm::dot(inner - p, before.Direction)) > before.Length * 0.5 ||
                    std::abs(glm::dot(inner - p, after.Direction)) > after.Length * 0.5;
                if (tight) inner = p;

                glm::dvec2& innerBefore = turn > 0 ? before.EndLeft : before.EndRight;
                glm::dvec2& innerAfter = turn > 0 ? after.StartLeft : after.StartRight;
                glm::dvec2& outerBefore = turn > 0 ? before.EndRight : before.EndLeft;
                glm::dvec2& outerAfter = turn > 0 ? after.StartRight : after.StartLeft;
                innerBefore = innerAfter = inner;

                if (style.Join == CurveJoin::Miter && !tight &&
                    glm::length(miter) <= halfWidth * style.MiterLimit) {
                    outerBefore = outerAfter = p - side * miter;
                } else if (style.Join == CurveJoin::Round) {
                    arc(p, inner, outerBefore, outerAfter, std::atan2(turn, cosine));
                } else {
                    triangle(inner, outerBefore, outerAfter); // Bevel / limite Miter.
                }
            }

            if (!contour.Closed) {
                Segment& first = segments.front();
                Segment& last = segments.back();
                if (style.Cap == CurveCap::Square) {
                    first.StartLeft -= first.Direction * halfWidth;
                    first.StartRight -= first.Direction * halfWidth;
                    last.EndLeft += last.Direction * halfWidth;
                    last.EndRight += last.Direction * halfWidth;
                } else if (style.Cap == CurveCap::Round) {
                    arc(points.front(), points.front(), first.StartLeft, first.StartRight, constants::Pi);
                    arc(points.back(), points.back(), last.EndRight, last.EndLeft, constants::Pi);
                }
            }
            for (const Segment& segment : segments) {
                triangle(segment.StartLeft, segment.StartRight, segment.EndRight);
                triangle(segment.StartLeft, segment.EndRight, segment.EndLeft);
            }
        }
        return mesh;
    }
    void RendererCurve2D::DrawCarthesian(const glm::dvec3& center, const glm::dvec3 size, const glm::dvec3 maille){

    }
    void RendererCurve2D::DrawCarthesian(const glm::dvec2& center, const glm::dvec2 size, const glm::dvec2 maille, const CurveStyle& style, const glm::dvec2 depassement){

        for(int i = -(int) size.x; i <= (int) size.x; i++){
            glm::dvec2 start = center + (glm::dvec2) {i*maille.x, maille.y*(size.y + depassement.y)};
            glm::dvec2 end = center + (glm::dvec2) {i*maille.x, -maille.y*(size.y + depassement.y)};
            DrawLine(start, end, s_CurveStyle);
        }
        for(int i = -(int) size.y; i <= (int) size.y; i++){
            glm::dvec2 start = center + (glm::dvec2) {maille.x*(size.x + depassement.x), i*maille.y};
            glm::dvec2 end = center + (glm::dvec2) {-maille.x*(size.x + depassement.x), i*maille.y };
            DrawLine(start, end, style);
        }
    }
    void RendererCurve2D::DrawCarthesian(const glm::dvec2& center, const glm::dvec2 size, const glm::dvec2 maille,const glm::dvec2 depassement){
        DrawCarthesian(center,size,maille, s_CurveStyle,depassement);
    }

}