#pragma once

#include <cstdint>
#include <vector>

namespace scivibe {
    struct CapturedFrame {
        int Width = 0;
        int Height = 0;
        int StrideBytes = 0;
        std::vector<uint8_t> Pixels;
    };
}