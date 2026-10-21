#pragma once

#include "objet/objet2D.hpp"
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

    class Animator {
    public:
        Animator(Object2D& object,const Object2D& destination,double duration)
            : m_Object(object),
            m_Start(Object2DState::Capture(object)),
            m_End(Object2DState::Capture(destination)),
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
        const float t = m_Progress;
        m_Object.m_Position = Interpolator<glm::vec3>::Interpolate(m_Start.Position, m_End.Position, t);
        m_Object.m_Rotation =Interpolator<float>::Interpolate(m_Start.Rotation, m_End.Rotation, t);
        m_Object.m_Scale =Interpolator<float>::Interpolate(m_Start.Scale, m_End.Scale, t);
        m_Object.m_Color = Interpolator<Color>::Interpolate( m_Start.Tint, m_End.Tint, t);
        m_Object.m_Visible = IsFinished() ? m_End.Visible : m_Start.Visible;
        m_Object.UpdateFromObject();
    }
        Object2D& m_Object;
        const Object2DState m_Start;
        const Object2DState m_End;
        const double m_Duration;
        double m_StartTime;
        float m_Progress = 0.0f;
    };
}
