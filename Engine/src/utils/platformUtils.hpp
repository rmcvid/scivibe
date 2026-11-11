#pragma once
#include <string>
#include "core/core.hpp"

namespace scivibe{
    class SCIVIBE_API FileDialogs{
    public:
        static std::string OpenFile(const char* filter);
        static std::string SaveFile(const char* filter);
    };
}