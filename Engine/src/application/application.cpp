#include "application/application.hpp"
#include "log/log.hpp"
#include "Events/event.hpp"
#include "Events/applicationEvent.hpp"
#include "pch/pch.hpp"
#include "GLFW/glfw3.h"
#include "window/windowInput.hpp"
#include "gui/imGuiLayer.hpp"
#include "renderer/renderer.hpp"
#include <cstdint>


namespace scivibe {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type){
        switch (type){
            case ShaderDataType::Float :    return GL_FLOAT;
            case ShaderDataType::Float2 :   return GL_FLOAT;
            case ShaderDataType::Float3 :   return GL_FLOAT;
            case ShaderDataType::Float4 :   return GL_FLOAT;
            case ShaderDataType::Mat3 :     return GL_FLOAT;
            case ShaderDataType::Mat4 :     return GL_FLOAT;
            case ShaderDataType::Int  :     return GL_INT;
            case ShaderDataType::Int2 :     return GL_INT;
            case ShaderDataType::Int3 :     return GL_INT;
            case ShaderDataType::Int4 :     return GL_INT;
            case ShaderDataType::Bool :     return GL_BOOL;
        }
        SCIVIBE_ASSERT(false, "Unknown ShaderType");
        return 0;
    }


    Application::Application()
    {
        SCIVIBE_CORE_ASSERT(!s_Instance, "Application already exist")

        s_Instance = this;
        m_window = std::unique_ptr<Window>(Window::Create());
        m_window->SetEventCallback(BIND_EVENT_FN(onEvent));
        //m_window->SetVSync(true);

        m_ImGuiLayer = new ImGuiLayer();
        PushOverLayer(m_ImGuiLayer); 

        
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
        layer->OnAttach();
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
        //SCIVIBE_CORE_INFO("Application running...");
        //WindowResizeEvent e(1280, 720);
        //SCIVIBE_TRACE(e);

        //Renderer::Flush();

        while (m_Running) {

            double time = glfwGetTime();
            Timestep deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;
            for (Layer* layer : m_LayerStack){
                layer->OnUpdate(deltaTime);
            }

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack){
                layer->OnImGuiRender();
            }
            m_ImGuiLayer->End();
            m_window->onUpdate();
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent &e){
        m_Running = false;
        return true;
    }


}
