#pragma once

#include <glm/glm.hpp>
#include "objet/color.hpp"

namespace scivibe {
    class Object2D {
    public:
        virtual ~Object2D() = default;
        glm::vec3 m_Position{0.0f};
        float m_Rotation = 0.0f;
        float m_Scale{1.0f};
        bool m_Visible = true;
        Color m_Color = Color::White;
        uint32_t m_ID; // ici il faudrait avoir un static non
        
        virtual void UpdateFromObject(){};
    };

}