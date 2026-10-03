#pragma once

#include "core/log.hpp"

namespace scivibe {
    class GraphicsContext{
        public:
            virtual void Init() =0;
            virtual void SwapBuffers()=0 ;
    };
}