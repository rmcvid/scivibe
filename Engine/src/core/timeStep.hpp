#pragma once
#include "core/core.hpp"
namespace scivibe{
    class SCIVIBE_API Timestep
    {
    public:     
        Timestep(double time = 0.0):
            m_Time(time)
        {}
        double GetSeconds() const {return m_Time;}
        double GetMiliseconds() const{ return m_Time*1000;}

        //operator float() const{ return static_cast<float>(m_Time);}
        operator double() const{ return m_Time;}

    private:
        double m_Time;
    };
}