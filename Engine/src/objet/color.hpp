
#pragma once

#include "core/core.hpp"
#include <glm/glm.hpp>

namespace scivibe {

    class Color {
    public:
        float r, g, b, a;
        constexpr Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
        constexpr Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
        Color(const glm::vec4& color) : r(color.r), g(color.g), b(color.b), a(color.a) {}
        glm::vec4 ToVec4() const {return {r, g, b, a};}
        constexpr Color operator+(const Color& other) const {return {r + other.r,g + other.g,b + other.b,a + other.a};}
        constexpr Color operator-(const Color& other) const {return {r - other.r,g - other.g,b - other.b,a - other.a};}
        constexpr Color operator*(float scalar) const {return {r * scalar,g * scalar,b * scalar,a * scalar};}
        constexpr Color operator/(float scalar) const { return {r / scalar,g / scalar,b / scalar,a / scalar};}
        Color& operator+=(const Color& other) {r += other.r;g += other.g;b += other.b;a += other.a;return *this;}
        Color& operator-=(const Color& other) {r -= other.r;g -= other.g;b -= other.b;a -= other.a;return *this;}
        Color& operator*=(float scalar) {r *= scalar;g *= scalar;b *= scalar;a *= scalar;return *this;}
        Color& operator/=(float scalar) {r /= scalar;g /= scalar;b /= scalar;a /= scalar;return *this;}
        constexpr bool operator==(const Color& other) const { return r == other.r && g == other.g && b == other.b && a == other.a;}
        constexpr bool operator!=(const Color& other) const { return !(*this == other);}
        static Color Lerp(const Color& start, const Color& end, float t) {return start + (end - start) * t;}
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color White;
        static const Color Black;
        static const Color Yellow;
        static const Color Transparent;
    };
    constexpr Color operator*(float scalar, const Color& color) {return color * scalar;}
    inline const Color Color::Red{1.0f, 0.0f, 0.0f};
    inline const Color Color::Green{0.0f, 1.0f, 0.0f};
    inline const Color Color::Blue{0.0f, 0.0f, 1.0f};
    inline const Color Color::White{1.0f, 1.0f, 1.0f};
    inline const Color Color::Black{0.0f, 0.0f, 0.0f};
    inline const Color Color::Yellow{1.0f, 1.0f, 0.0f};
    inline const Color Color::Transparent{0.0f, 0.0f, 0.0f, 0.0f};
}
