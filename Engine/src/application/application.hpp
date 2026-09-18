#pragma once
#include "core/core.hpp"
#include "Events/event.hpp"
#include "window/window.hpp"
#include "Events/applicationEvent.hpp"
#include "layer/layerStack.hpp"

namespace scivibe {
    class SCIVIBE_API Application {
    public:
        Application();
        virtual ~Application();

        void Run();
        void onEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverLayer(Layer* overlay);
    
    private :
        bool OnWindowClose(WindowCloseEvent& e);
        std::unique_ptr<Window> m_window;
        bool m_Running = true;
        LayerStack m_LayerStack;
    };
    // Optional factory implemented by the client, not exported by the engine.
    Application* CreateApplication();
}
