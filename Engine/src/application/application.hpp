#pragma once
#include "core/core.hpp"

namespace scivibe {
    class SCIVIBE_API Application {
    public:
        Application();
        virtual ~Application();

        void Run();
    };
    // Optional factory implemented by the client, not exported by the engine.
    Application* CreateApplication();
}
