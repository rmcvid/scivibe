#pragma once
#include "Events/event.hpp"
//#include "core.hpp"

namespace scivibe {
    class Application {
    public:
        Application();
        virtual ~Application();

        void Run();
    };
    Application* CreateApplication();
}