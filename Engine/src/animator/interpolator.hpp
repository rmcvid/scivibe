#pragma once

namespace scivibe {
    template<typename T>
    struct Interpolator {
        static T Interpolate(const T& start, const T& end, float t)
        {
            return start + (end - start) * t;
        }
    };
}