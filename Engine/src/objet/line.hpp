#pragma once
#include "objet/objet2D.hpp"
#include "objet/color.hpp"
#include "core/core.hpp"

namespace scivibe {
    class Line : public Object2D {
    public:
        glm::vec2 m_Start{0.0f};
        glm::vec2 m_End{1.0f};
        float m_Thickness = 2.0f;
        Line() = default;
        Line(const glm::vec2& start,const glm::vec2& end,const Color& color = Color::White,float thickness = 1.0f)
            : Object2D(),m_Start(start), m_End(end), m_Thickness(thickness) 
        {   
            m_Color = color;
            m_Position = {start.x,start.y,0.0f};
            glm::vec2 direction = end - start;
            m_Rotation = glm::atan(direction.y, direction.x);
            m_Scale = glm::length(m_Start - m_End);
        }
        void UpdateFromObject() override{
            m_Start = glm::vec2(m_Position);
            m_End = m_Start + m_Scale * glm::vec2{glm::cos(m_Rotation),glm::sin(m_Rotation)};
        }
    };

}