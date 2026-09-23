#pragma once
#include "core/core.hpp"
#include "Events/event.hpp"
#include "window/window.hpp"
#include "Events/applicationEvent.hpp"
#include "layer/layerStack.hpp"
#include "gui/imGuiLayer.hpp"
#include "shader/shader.hpp"
#include "renderer/buffer.hpp"
#include "renderer/vertexArray.hpp"

namespace scivibe {
    class SCIVIBE_API Application {
    public:
        Application();
        virtual ~Application();

        void Run();
        void onEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverLayer(Layer* overlay);
        inline static Application& Get(){ return *s_Instance; };
        inline Window& GetWindow(){return *m_window;};
    private :
        bool OnWindowClose(WindowCloseEvent& e);
        std::unique_ptr<Window> m_window;
        ImGuiLayer* m_ImGuiLayer;
        bool m_Running = true;
        LayerStack m_LayerStack;

        std::shared_ptr<Shader> m_Shader; 
        std::shared_ptr<VertexArray> m_VertexArray;

        static Application* s_Instance;
    };
    // Optional factory implemented by the client, not exported by the engine.
    Application* CreateApplication();
}
