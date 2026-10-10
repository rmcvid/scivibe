#include "core/application.hpp"
#include "core/log.hpp"
#include "Events/event.hpp"
#include "Events/applicationEvent.hpp"
#include "pch/pch.hpp"
#include "GLFW/glfw3.h"
#include "window/windowInput.hpp"
#include "gui/imGuiLayer.hpp"
#include "renderer/renderer.hpp"
#include "renderer/renderer2D.hpp"
#include <cstdint>
#include <cmath>

namespace scivibe {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;
    Timestep Application::s_DeltaTime{0.0};
    Application::FrameRateData Application::s_FrameRateData{};
    
    

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

        m_WindowRecorder = Scope<WindowRecorder>(WindowRecorder::Create());

        Renderer::Init();
        m_ImGuiLayer = new ImGuiLayer();
        PushOverLayer(m_ImGuiLayer); 

        
    }
    Application::~Application() { 
        // Release static GPU resources while the window's GL context still exists.
        Renderer2D::Shutdown();
        m_WindowRecorder.reset();
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

    void Application::StartRecording(const std::string& filename, int fps){
        /*ici plutot donner les information au window recorder non ?*/
        // Mettre a jour l'etat seulement si le recorder demarre correctement.
        m_WindowRecorder->SetCaptureData( m_window->GetWidth(), m_window->GetHeight(), fps,glfwGetTime());
        m_WindowRecorder->StartRecording(filename);
    }
    void Application::StopRecording(){
        m_WindowRecorder->StopRecording();
    }
    void Application::RecordFrame(){
        const int64_t pts = static_cast<int64_t>(
                    (glfwGetTime() - m_WindowRecorder->GetRecordingStartTime()) *m_WindowRecorder->GetFPS());
        if (pts > m_WindowRecorder->GetLastRecordingPts() && m_window->CaptureFrame(m_CapturedFrame)){
            const auto& frame = m_CapturedFrame;
            if (frame.Width != m_WindowRecorder->GetWidth() || frame.Height != m_WindowRecorder->GetHeight()) {
                StopRecording();
            }
            else {
                const uint8_t* topRow = frame.Pixels.data() + static_cast<size_t>(frame.Height - 1) * frame.StrideBytes;
                m_WindowRecorder->RecordFrame(topRow,-frame.StrideBytes,pts);
                m_WindowRecorder->SetLastRecordingPts(pts);
            }
        }
    }

    void Application::Run(){
        while (m_Running) {
            const double time = glfwGetTime();
            m_window->PollEvents();
            if (!m_Running) break;
            frameRateDataCalcul(time);
            if(!m_Minimized){
                for (Layer* layer : m_LayerStack){
                   layer->OnUpdate(s_DeltaTime);
                }    
            }

            if(m_WindowRecorder->IsRecording()){
                RecordFrame();
            }

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack){
                layer->OnImGuiRender();
            }
            m_ImGuiLayer->End();
            m_window->SwapBuffers();
            AppWait();
        }
    }

    void Application::frameRateDataCalcul(double time){
        s_DeltaTime = s_FrameRateData.firstFrame ? 0.0 : time - m_LastFrameTime;
            m_LastFrameTime = time;
            s_FrameRateData.firstFrame = false;
            if (s_DeltaTime.GetSeconds() > 0.0) {
                s_FrameRateData.sampleSeconds += s_DeltaTime.GetSeconds();
                ++s_FrameRateData.sampleFrames;
                if (s_FrameRateData.sampleSeconds >= 0.5) {
                    s_FrameRateData.FPS = static_cast<int>(std::lround(s_FrameRateData.sampleFrames / s_FrameRateData.sampleSeconds));
                    s_FrameRateData.AverageFrameMilliseconds = 1000.0 * s_FrameRateData.sampleSeconds / s_FrameRateData.sampleFrames;
                    s_FrameRateData.sampleSeconds = 0.0;
                    s_FrameRateData.sampleFrames = 0;
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

   void Application::AppWait() {
    if (s_FrameRateData.TargetFPS <= 0) {s_FrameRateData.NextFrameTime = 0.0;s_FrameRateData.PreviousTargetFPS = 0;return;}
    const double interval = 1.0 / static_cast<double>(s_FrameRateData.TargetFPS);
    if (s_FrameRateData.NextFrameTime == 0.0 || s_FrameRateData.TargetFPS != s_FrameRateData.PreviousTargetFPS) { 
        s_FrameRateData.NextFrameTime = m_LastFrameTime + interval;
    } else {
        s_FrameRateData.NextFrameTime += interval;
    }
    const double now = glfwGetTime();
    if (s_FrameRateData.NextFrameTime < now) s_FrameRateData.NextFrameTime = now;
    s_FrameRateData.PreviousTargetFPS = s_FrameRateData.TargetFPS;
    while (m_Running) {
        const double remaining = s_FrameRateData.NextFrameTime - glfwGetTime();
        if (remaining <= 0.0)
            break;
        m_window->WaitEvents(remaining);
    }
}

}
