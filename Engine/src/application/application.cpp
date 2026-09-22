#include "application/application.hpp"
#include "log/log.hpp"
#include "Events/event.hpp"
#include "Events/applicationEvent.hpp"
#include "pch/pch.hpp"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "window/windowInput.hpp"
#include "gui/imGuiLayer.hpp"

namespace scivibe {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application() {
        SCIVIBE_CORE_ASSERT(!s_Instance, "Application already exist")
        s_Instance = this;
        m_window = std::unique_ptr<Window>(Window::Create());
        m_window->SetEventCallback(BIND_EVENT_FN(onEvent));

        m_ImGuiLayer = new ImGuiLayer();
        PushOverLayer(m_ImGuiLayer); 

        glGenVertexArrays(1,&m_VertexArray);
        glBindVertexArray(m_VertexArray);

        float vertices[3*3] {
            -0.5f,-0.5f, 0.0f,
             0.5f,-0.5f, 0.0f,
            -0.0f, 0.5f, 0.0f,
        };

        m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
        m_VertexBuffer->Bind();
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 3*sizeof(float), nullptr);

        uint32_t indices[3] = {0,1,2};
        m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)/ sizeof(uint32_t)));

        m_Shader.reset(new Shader(
            SHADER_PATH "shader.vs",
            SHADER_PATH "shader.fs"
        ));

        // vertex array
        // vertex buffer
        // index buffer

        // shader
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
        SCIVIBE_CORE_INFO("Application running...");
        WindowResizeEvent e(1280, 720);
        SCIVIBE_TRACE(e);

        while (m_Running) {
            glClearColor(0,0,0,1);
            glClear(GL_COLOR_BUFFER_BIT);

            m_Shader->Bind();
            glBindVertexArray(m_VertexArray);
            glDrawElements(GL_TRIANGLES,m_IndexBuffer->GetCount(),GL_UNSIGNED_INT,nullptr);
            for (Layer* layer : m_LayerStack){
                layer->OnUpdate();
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
