#pragma once
#include "core/core.hpp"
#include "core/timeStep.hpp"
#include "Events/event.hpp"
#include "window/window.hpp"
#include "Events/applicationEvent.hpp"
#include "layer/layerStack.hpp"
#include "gui/imGuiLayer.hpp"
#include "shader/shader.hpp"
#include "renderer/buffer.hpp"
#include "renderer/vertexArray.hpp"
#include "renderer/camera.hpp"
#include "media/recorder.hpp"
#include "constants/constant.hpp"
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
        void InitRecording(const std::string& filename, int fps = FPS_RECORD);
        void StartRecording();
        void RecordFrame();
        void StopRecording();
        static Timestep GetDeltaTime(){ return s_DeltaTime;}
        int GetTargetFPS() const { return s_FrameRateData.TargetFPS; }
        // Zero disables the software limit; VSync is controlled independently.
        void SetTargetFPS(int fps) { s_FrameRateData.TargetFPS = fps > 0 ? fps : 0; }
        int GetFPS() const { return s_FrameRateData.FPS; }
        double GetAverageFrameMilliseconds() const { return s_FrameRateData.AverageFrameMilliseconds; }
        bool IsRecording() const {return m_Recorder->IsRecording();}
        
    private :
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);
        void frameRateDataCalcul(double time);
        void AppWait();

        ImGuiLayer* m_ImGuiLayer;
        Scope<Window> m_window;
        Scope<Recorder> m_Recorder;
        
        bool m_Running = true;
        bool m_Minimized = false;
        LayerStack m_LayerStack;
        static Timestep s_DeltaTime;
        
        double m_LastFrameTime {0.0};
        static Application* s_Instance;

        struct FrameRateData {
            bool firstFrame = true;
            double sampleSeconds = 0.0;
            int sampleFrames = 0;
            int TargetFPS = TARGET_FPS;
            int PreviousTargetFPS = TARGET_FPS;
            int FPS = 0;
            double NextFrameTime = 0.0;
            double AverageFrameMilliseconds = 0.0;
        };
        static FrameRateData s_FrameRateData;
        CapturedFrame m_CapturedFrame;
    };
    Application* CreateApplication();
}
