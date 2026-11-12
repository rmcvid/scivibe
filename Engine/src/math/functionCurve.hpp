#pragma once
#include "core/core.hpp"
#include "renderer/rendererCurve2D.hpp"
#include <functional>
#include <string>
#include <string_view>

namespace scivibe::Math {
    using ScalarFunction = std::function<double(double)>;

    // Supported: x, pi, e, + - * / ^, parentheses and unary math functions.
    // Invalid expressions return an empty function and an error for the editor.
    SCIVIBE_API ScalarFunction CompileFunction(std::string_view expression, std::string& error);
    // The same sampler can be used directly with a C++ lambda.
    SCIVIBE_API CurvePath SampleFunction(const ScalarFunction& function,
        double minX, double maxX, int segments = 512);
}
