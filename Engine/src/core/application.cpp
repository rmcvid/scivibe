#include "core/application.hpp"
#include "core/log.hpp"
#include "Events/event.hpp"
#include "Events/applicationEvent.hpp"
#include "pch/pch.hpp"
#include "GLFW/glfw3.h"
#include "window/windowInput.hpp"
#include "gui/imGuiLayer.hpp"
#include "renderer/renderer.hpp"
#include <cstdint>
#include <chrono>
#include <thread>

namespace scivibe {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;
    Timestep Application::s_DeltaTime{0.0};

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
        m_window = Scope<Window>(Window::Create());
        m_window->SetEventCallback(BIND_EVENT_FN(onEvent));

        Renderer::Init();
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
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

        for( auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ){
            (*--it)->OnEvent(e);
            if(e.GetHandled()){
                break;
            }
        }
    }

    void Application::Run() {
        bool firstFrame = true;
        m_LastFrameTime = glfwGetTime();
        while (m_Running) {

            const double time = glfwGetTime();
            s_DeltaTime = firstFrame ? 0.0 : time - m_LastFrameTime;
            m_LastFrameTime = time;
            firstFrame = false;
            if(!m_Minimized){
                for (Layer* layer : m_LayerStack){
                   layer->OnUpdate(s_DeltaTime);
                }
                
            }

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack){
                layer->OnImGuiRender();
            }
            m_ImGuiLayer->End();
            m_window->onUpdate();

            if (m_Running && targetFPS > 0) {
                const double targetDuration = 1.0 / targetFPS;
                const double elapsed = glfwGetTime() - time;
                const double remaining = targetDuration - elapsed;

                if (remaining > 0.0) {
                    std::this_thread::sleep_for( std::chrono::duration<double>(remaining));
                }
            }
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent &e){
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent &e){
        if(e.GetWidth() == 0 || e.GetHeight() == 0){
            m_Minimized = true;
            return false;
        }
        m_Minimized = false;
        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
        return false;
    }

}
