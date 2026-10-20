#pragma once
#include "pch/pch.hpp"
#include "core/core.hpp"


namespace scivibe {
    class SCIVIBE_API Input {
        public:
            static bool IsKeyPressed(int keycode);
            static bool IsMouseButtonPressed(int button);
            static std::pair<float,float> GetMousePosition();
            static bool GetMouseX();
            static bool GetMouseY();
    };
}