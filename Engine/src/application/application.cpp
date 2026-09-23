#include "application/application.hpp"
#include "log/log.hpp"
#include "Events/event.hpp"
#include "Events/applicationEvent.hpp"
#include "pch/pch.hpp"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "window/windowInput.hpp"
#include "gui/imGuiLayer.hpp"
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


    Application::Application() {
        SCIVIBE_CORE_ASSERT(!s_Instance, "Application already exist")
        s_Instance = this;
        m_window = std::unique_ptr<Window>(Window::Create());
        m_window->SetEventCallback(BIND_EVENT_FN(onEvent));

        m_ImGuiLayer = new ImGuiLayer();
        PushOverLayer(m_ImGuiLayer); 

        m_VertexArray.reset(VertexArray::Create());
        float vertices[3*7] {
            -0.5f,-0.5f, 0.0f, 1.0f,1.0f, 1.0f ,1.0f,
             0.5f,-0.5f, 0.0f, 0.2f,0.3f,0.8f,1.0f,
            -0.0f, 0.5f, 0.0f, 0.1f,0.9f,0.2f,1.0f
        };
        std::shared_ptr<VertexBuffer> vertexBuffer;
        vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
        BufferLayout layout {
            {ShaderDataType::Float3, "aPosition"},
            {ShaderDataType::Float4, "aColor"}
        };
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[3] = {0,1,2};
        std::shared_ptr<IndexBuffer> indexBuffer;
        indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)/ sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);
        m_Shader.reset(new Shader(
            SHADER_PATH "shader.vs",
            SHADER_PATH "shader.fs"
        ));
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
            m_VertexArray->Bind();
            glDrawElements(GL_TRIANGLES,m_VertexArray->GetIndexBuffers()->GetCount(),GL_UNSIGNED_INT,nullptr);
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
