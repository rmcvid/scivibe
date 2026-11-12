#include "scene/component.hpp"
#include "math/functionCurve.hpp"
#include <cmath>

namespace scivibe {
    bool SpriteRendererComponent::SetTexture(const std::string& path) {
        try {
            auto texture = Texture2D::Create(path);
            if (!texture) { TextureError = "Impossible de charger la texture."; return false; }
            Texture = std::move(texture);
            TexturePath = path;
            TextureError.clear();
            return true;
        } catch (const std::exception& error) {
            TextureError = error.what();
            return false;
        }
    }

    const CurvePath& FunctionCurveComponent::GetPath() {
        if (m_Cached && Expression == m_CachedExpression && MinX == m_CachedMinX &&
            MaxX == m_CachedMaxX && Segments == m_CachedSegments) return m_Path;
        m_Cached = true;
        m_CachedExpression = Expression;
        m_CachedMinX = MinX;
        m_CachedMaxX = MaxX;
        m_CachedSegments = Segments;
        m_Path.Contours.clear();
        m_Error.clear();
        if (!std::isfinite(MinX) || !std::isfinite(MaxX) || MinX >= MaxX) {
            m_Error = "Intervalle invalide : X min doit etre inferieur a X max.";
            return m_Path;
        }
        if (Segments < 2 || Segments > 16384) {
            m_Error = "Le nombre de segments doit etre compris entre 2 et 16384.";
            return m_Path;
        }
        auto function = Math::CompileFunction(Expression, m_Error);
        if (function) m_Path = Math::SampleFunction(function, MinX, MaxX, Segments);
        return m_Path;
    }
}
