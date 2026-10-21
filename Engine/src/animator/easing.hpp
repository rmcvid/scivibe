#pragma once
#include "pch/pch.hpp"
namespace scivibe {
    enum class EasingType {
        Linear,
        Smoothstep,
        EaseInOutCubic
    };
    class Easing {
    public:
        static float Apply(float t, EasingType type) {
            t = std::clamp(t, 0.0f, 1.0f);
            switch (type) {
                case EasingType::Linear:
                    return t;
                case EasingType::Smoothstep:
                    return t * t * (3.0f - 2.0f * t);
                case EasingType::EaseInOutCubic:
                    return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
            }
            return t;
        }
    };
}