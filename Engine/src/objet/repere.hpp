#pragma once
#include "objet/objet2D.hpp"
#include "objet/color.hpp"
#include "core/core.hpp"
#include "constants/constant.hpp"

namespace scivibe {
    
    class Repere : public Object2D {
    public:
        enum class Type{
            carthesian,
            polar,
        };
        glm::vec2 m_maille;
        glm::vec2 m_size;
        float m_Thickness = 2.0f;
        Type m_type;
        glm::vec2 m_Direction; // donne la direction des vecteur de base en radiant, pas applicable au repere polaire
        glm::dvec2 m_depassement;

        Repere() = default;
        void UpdateFromObject() override {};
        virtual void UpdateFromRepere(){};
    };
    class Carthesian : public Repere {
        public:
            Carthesian(){
            }
            Carthesian(const glm::dvec2& center, const glm::dvec2 size,const glm::vec2 direction = {0, constants::Pi/2}, const glm::dvec2 maille = {1,1}, const glm::dvec2 depassement = {0,0}, const Color color = Color::White)
            {
                m_Position = {center.x,center.y, 0.0f};
                m_size = size;
                m_Direction = direction;
                m_maille = maille;
                m_depassement = depassement;
                m_Color = color;
                m_type = Type::carthesian;
            }
            void UpdateFromObject() override{
            }
            void UpdateFromRepere() override{

            }

    };

}