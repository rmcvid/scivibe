#include "application/application.hpp"
#include "log/log.hpp"
#include "Events/event.hpp"
#include "Events/applicationEvent.hpp"
#include "pch/pch.hpp"
#include "GLFW/glfw3.h"
#include "glad/glad.h"

namespace scivibe {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;
    Application::Application() {
        SCIVIBE_CORE_ASSERT(!s_Instance, "Application already exist")
        s_Instance = this;
        m_window = std::unique_ptr<Window>(Window::Create());
        m_window->SetEventCallback(BIND_EVENT_FN(onEvent));
        SCIVIBE_CORE_INFO("Application created");
    }

    Application::~Application() {
        SCIVIBE_CORE_INFO("Application destroyed");
    }

    
    void Application::PushLayer(Layer* layer){
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverLayer(Layer* layer){
        m_LayerStack.PushOverLayer(layer);
    }

    void Application::onEvent(Event& e){
        EventDispatcher dispatcher(e); 
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
        SCIVIBE_CORE_TRACE("{0}",e);
        for( auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ){
            (*--it)->OnEvent(e);
            if(e.GetHandled()){
                break;
            }
        }
    }

    void Application::Run() {
        SCIVIBE_CORE_INFO("Application running...");
        WindowResizeEvent e(1280, 720);
        SCIVIBE_TRACE(e);
        while (m_Running) {
            glClearColor(1,0,1,1);
            glClear(GL_COLOR_BUFFER_BIT);
            for (Layer* layer : m_LayerStack){
                layer->OnUpdate();
            }
            m_window->onUpdate();
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent &e){
        m_Running = false;
        return true;
    }


}
