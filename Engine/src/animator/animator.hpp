#pragma once

#include "objet/objet2D.hpp"
#include "objet/repere.hpp"
#include "animator/interpolator.hpp"
#include "core/timeStep.hpp"
#include "pch/pch.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace scivibe {

    struct Object2DState {
        glm::vec3 Position;
        float Rotation;
        float Scale;
        Color Tint;
        bool Visible;

        static Object2DState Capture(const Object2D& object){
            return {object.m_Position,object.m_Rotation,object.m_Scale,object.m_Color,object.m_Visible};
        }
    };

    struct Repere2DState {
        Object2DState Common;
        glm::vec2 Maille;
        glm::vec2 Size;
        glm::vec2 Direction;
        glm::dvec2 Depassement;
        float Thickness;
        Repere::Type Type;
    };
    template<typename T>
    struct AnimationTraits;
    template<>
    struct AnimationTraits<Object2D> {
        using State = Object2DState;
        static State Capture(const Object2D& object){
            return Object2DState::Capture(object);
        }
        static void ApplyProperties(Object2D& object,const State& start,const State& end,float t){
            object.m_Position = Interpolator<glm::vec3>::Interpolate(start.Position, end.Position, t);
            object.m_Rotation =Interpolator<float>::Interpolate(start.Rotation, end.Rotation, t);
            object.m_Scale =Interpolator<float>::Interpolate(start.Scale, end.Scale, t);
            object.m_Color =Interpolator<Color>::Interpolate(start.Tint, end.Tint, t);
            object.m_Visible = t >= 1.0f? end.Visible: start.Visible;
        }

        static void Apply(Object2D& object,const State& start,const State& end,float t){
            ApplyProperties(object, start, end, t);object.UpdateFromObject();
        }
    };
    template<>
    struct AnimationTraits<Repere> {
        using State = Repere2DState;
        static State Capture(const Repere& object){
            return { AnimationTraits<Object2D>::Capture(object),
                object.m_maille,object.m_size,object.m_Direction,object.m_depassement,
                object.m_Thickness,object.m_type
            };
        }
        static void Apply( Repere& object, const State& start, const State& end, float t){
            if (start.Type != end.Type) {
                throw std::invalid_argument( "Les reperes doivent avoir le meme type" );
            }
            AnimationTraits<Object2D>::ApplyProperties( object, start.Common, end.Common, t);
            object.m_maille =Interpolator<glm::vec2>::Interpolate(start.Maille, end.Maille, t);
            object.m_size =Interpolator<glm::vec2>::Interpolate(start.Size, end.Size, t);
            object.m_Direction =Interpolator<glm::vec2>::Interpolate(start.Direction, end.Direction, t);
            object.m_depassement = glm::mix(start.Depassement,end.Depassement,static_cast<double>(t));
            object.m_Thickness = Interpolator<float>::Interpolate( start.Thickness, end.Thickness, t);
            object.m_type = start.Type;
            object.UpdateFromObject();
            object.UpdateFromRepere();
        }
    };

   template<typename T>
    class Animator {
        using Traits = AnimationTraits<T>;
        using State = typename Traits::State;

    public:
        Animator(T& object, const T& destination, double duration = 1.0)
            : m_Object(object),
            m_Start(Traits::Capture(object)),
            m_End(Traits::Capture(destination)),
            m_Duration(duration),
            m_StartTime(Timestep::GetTime())
        {
            if (!std::isfinite(duration) || duration <= 0.0)
                throw std::invalid_argument("La duree doit etre positive et finie");
        }
    void Update(){
        if (IsFinished())return;
        const double elapsed = Timestep::GetTime() - m_StartTime;
        m_Progress = static_cast<float>(std::clamp(elapsed / m_Duration, 0.0, 1.0));
        Apply();
    }
    void Reset(){
        m_StartTime = Timestep::GetTime();
        m_Progress = 0.0f;
        Apply();
    }
    bool IsFinished() const{return m_Progress >= 1.0f;}

    private:
        void Apply(){
            Traits::Apply(m_Object, m_Start, m_End, m_Progress);
        }
        T& m_Object;
        const State m_Start;
        const State m_End;
        const double m_Duration;
        double m_StartTime;
        float m_Progress = 0.0f;
    };
}
