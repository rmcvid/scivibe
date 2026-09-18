#pragma once
#include "core/core.hpp"
#include "Events/event.hpp"
#include "window/window.hpp"
namespace scivibe {
    class SCIVIBE_API Application {
    public:
        Application();
        virtual ~Application();

        void Run();
    
    private :
        std::unique_ptr<Window> m_window;
        bool m_Running = true;
    };
    // Optional factory implemented by the client, not exported by the engine.
    Application* CreateApplication();
}
