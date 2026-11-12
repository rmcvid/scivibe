#include "renderer/rendererCurve2D.hpp"
#include "renderer/renderer2D.hpp"
#include "pch/pch.hpp"
#include "core/log.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <cmath>
#include <limits>

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
        DrawPath(path, style, glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, z)));
    }
    void RendererCurve2D::DrawPath(const CurvePath& path,const CurveStyle& style,const glm::mat4& transform){
        const CurveMesh mesh = BuildStroke(path, style, transform);
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
        s_CurveView = glm::dmat4(viewProjection);
        s_CurveViewport = framebufferSize;
        s_CurveViewReady = true;
    }
    CurveMesh RendererCurve2D::BuildStroke( const CurvePath& path, const CurveStyle& style, const glm::mat4& transform){
        CurveMesh mesh;
        if (!s_CurveViewReady) throw std::logic_error("Appeler SetView avant BuildStroke");
        if (!std::isfinite(style.WidthPixels) || !std::isfinite(style.MiterLimit) || style.MiterLimit < 1.0f)
            throw std::invalid_argument("Style de courbe invalide");
        if (style.WidthPixels <= 0.0f || s_CurveViewport.x == 0 || s_CurveViewport.y == 0) return mesh;
        const double halfWidth = style.WidthPixels * 0.5;
        const glm::dvec2 viewport(s_CurveViewport);
        const glm::dmat4 model(transform);
        const glm::dmat4 clipFromLocal = s_CurveView * model;
        // Homography of the local XY plane. Keeping W allows perspective views
        // while the existing tessellator still works with constant pixel widths.
        auto pixelColumn = [&](int column) {
            const auto c = clipFromLocal[column];
            return glm::dvec3((c.x + c.w) * viewport.x * 0.5,
                             (c.y + c.w) * viewport.y * 0.5, c.w);
        };
        const glm::dmat3 toPixels(pixelColumn(0), pixelColumn(1), pixelColumn(3));
        const double determinant = glm::determinant(toPixels);
        const double magnitude = glm::length(toPixels[0]) * glm::length(toPixels[1]) * glm::length(toPixels[2]);
        // A plane seen edge-on (or scaled to zero) has no drawable area.
        if (!std::isfinite(determinant) || std::abs(determinant) <= 1e-12 * magnitude) return mesh;
        const glm::dmat3 toLocal = glm::inverse(toPixels);

        CurvePath screenPath;
        const glm::dvec2 margin = (halfWidth * std::max(1.0f, style.MiterLimit) + 2.0) * 2.0 / viewport;
        auto clipSegment = [&](const glm::dvec4& a, const glm::dvec4& b, double& first, double& last) {
            const auto distances = [&](const glm::dvec4& p) {
                return std::array<double, 7>{p.w - 1e-7, p.z + p.w, p.w - p.z,
                    p.x + (1.0 + margin.x) * p.w, (1.0 + margin.x) * p.w - p.x,
                    p.y + (1.0 + margin.y) * p.w, (1.0 + margin.y) * p.w - p.y};
            };
            const auto da = distances(a), db = distances(b);
            first = 0.0; last = 1.0;
            for (size_t i = 0; i < da.size(); ++i) {
                if (!std::isfinite(da[i]) || !std::isfinite(db[i])) return false;
                if (da[i] < 0.0 && db[i] < 0.0) return false;
                if ((da[i] < 0.0) != (db[i] < 0.0)) {
                    const double t = da[i] / (da[i] - db[i]);
                    if (da[i] < 0.0) first = std::max(first, t);
                    else last = std::min(last, t);
                }
            }
            return first <= last;
        };
        auto project = [&](const glm::dvec4& p) {
            return (glm::dvec2(p) / p.w + glm::dvec2(1.0)) * viewport * 0.5;
        };
        // Clip before division by W. Split invisible runs instead of connecting
        // points through the camera or across a hidden section of a closed path.
        for (const auto& contour : path.Contours) {
            if (contour.Points.size() < 2) continue;
            std::vector<CurveContour> runs;
            CurveContour run;
            auto finish = [&]() {
                if (run.Points.size() >= 2) runs.push_back(std::move(run));
                run = CurveContour{};
            };
            const size_t count = contour.Closed ? contour.Points.size() : contour.Points.size() - 1;
            for (size_t i = 0; i < count; ++i) {
                const auto a = clipFromLocal * glm::dvec4(contour.Points[i], 0.0, 1.0);
                const auto b = clipFromLocal * glm::dvec4(contour.Points[(i + 1) % contour.Points.size()], 0.0, 1.0);
                double first, last;
                if (!clipSegment(a, b, first, last)) { finish(); continue; }
                const glm::dvec2 start = project(a + (b - a) * first);
                const glm::dvec2 end = project(a + (b - a) * last);
                if (!run.Points.empty() && glm::length(start - run.Points.back()) > PointEpsilon) finish();
                if (run.Points.empty()) run.Points.push_back(start);
                if (glm::length(end - run.Points.back()) > PointEpsilon) run.Points.push_back(end);
                if (last < 1.0) finish();
            }
            finish();
            if (contour.Closed && runs.size() > 1 &&
                glm::length(runs.back().Points.back() - runs.front().Points.front()) <= PointEpsilon) {
                auto merged = std::move(runs.back().Points);
                merged.insert(merged.end(), runs.front().Points.begin() + 1, runs.front().Points.end());
                runs.front().Points = std::move(merged);
                runs.pop_back();
            }
            for (auto& part : runs) {
                if (contour.Closed && glm::length(part.Points.front() - part.Points.back()) <= PointEpsilon) {
                    part.Closed = true;
                    part.Points.pop_back();
                }
                screenPath.Contours.push_back(std::move(part));
            }
        }

        auto vertex = [&](const glm::dvec2& p, glm::vec3& result) {
            const glm::dvec3 local = toLocal * glm::dvec3(p, 1);
            if (!std::isfinite(local.z) || local.z <= 1e-12) return false;
            const glm::dvec4 world = model * glm::dvec4(local.x / local.z, local.y / local.z, 0.0, 1.0);
            const double maxFloat = (std::numeric_limits<float>::max)();
            for (int i = 0; i < 3; ++i)
                if (!std::isfinite(world[i]) || std::abs(world[i]) > maxFloat) return false;
            result = glm::vec3(world);
            return true;
        };
        auto triangle = [&](glm::dvec2 a, glm::dvec2 b, glm::dvec2 c) {
            const double area = Cross(b - a, c - a);
            if (std::abs(area) < 1e-12) return;
            if (area < 0) std::swap(b, c); // Sens antihoraire a l'ecran.
            glm::vec3 wa, wb, wc;
            if (!vertex(a, wa) || !vertex(b, wb) || !vertex(c, wc)) return;
            if (mesh.Positions.size() > (std::numeric_limits<uint32_t>::max)() - 3u)
                throw std::overflow_error("Maillage de courbe trop grand");
            const auto base = static_cast<uint32_t>(mesh.Positions.size());
            mesh.Positions.push_back(wa);
            mesh.Positions.push_back(wb);
            mesh.Positions.push_back(wc);
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
        for (const CurveContour& contour : screenPath.Contours) {
            std::vector<glm::dvec2> points;
            for (const glm::dvec2& p : contour.Points) {
                if (!std::isfinite(p.x) || !std::isfinite(p.y))
                    throw std::invalid_argument(
                        "Point non fini : separer les branches avant BuildStroke");
                const glm::dvec2 screen = p;
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
    void RendererCurve2D::DrawCarthesian(const glm::dvec2& center,const glm::dvec2 size,const glm::dvec2 maille,const CurveStyle& style,const glm::dvec2 depassement,const glm::vec2 direction,const glm::mat4& transform){
        if (!std::isfinite(size.x) || !std::isfinite(size.y) || size.x < 0 || size.y < 0 ||
            size.x > 1000 || size.y > 1000 || !std::isfinite(maille.x) || !std::isfinite(maille.y) ||
            maille.x <= 0 || maille.y <= 0) return;
        const glm::dvec2 dirX(std::cos(direction.x),std::sin(direction.x));
        const glm::dvec2 dirY(std::cos(direction.y), std::sin(direction.y));
        CurvePath grid;
        const double lengthX = maille.x * (size.x + depassement.x);
        const double lengthY = maille.y * (size.y + depassement.y);
        for (int i = -static_cast<int>(size.x); i <= static_cast<int>(size.x); ++i){
            const glm::dvec2 offset =  static_cast<double>(i) * maille.x * dirX;
            const glm::dvec2 start = center + offset - lengthY * dirY;
            const glm::dvec2 end   = center + offset + lengthY * dirY;
            grid.Contours.push_back(CurveContour{{start, end}, false});
        }
        for (int i = -static_cast<int>(size.y); i <= static_cast<int>(size.y); ++i){
            const glm::dvec2 offset = static_cast<double>(i) * maille.y * dirY;
            const glm::dvec2 start = center + offset - lengthX * dirX;
            const glm::dvec2 end   = center + offset + lengthX * dirX;
            grid.Contours.push_back(CurveContour{{start, end}, false});
        }
        DrawPath(grid, style, transform);
    }
    void RendererCurve2D::DrawRepere(const Repere& repere, const CurveStyle& style){
        switch (repere.m_type)
        {
        case Repere::Type::carthesian :
            CurveStyle styleCopie = style;
            styleCopie.WidthPixels = repere.m_Thickness;
            DrawCarthesian(glm::dvec2(repere.m_Position), repere.m_size, repere.m_maille,
                styleCopie, repere.m_depassement, repere.m_Direction,
                glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, repere.m_Position.z)));
            break;
        /*case Repere::Type::polar:
            SCIVIBE_CORE_INFO("le repère polaire n'est pas encore écrit");
            break;
        default:
            break;*/
        }
    }

}
