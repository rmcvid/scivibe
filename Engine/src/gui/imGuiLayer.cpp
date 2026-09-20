#include "pch/pch.hpp"
#include "core/core.hpp"
#include "imGuiLayer.hpp"
#include "imgui.h"
#include "plateform/OpenGl/imguiOpenGLRenderer.hpp"
#include "application/application.hpp"

// glat and glfw does not need to be included ici
namespace scivibe{
    ImGuiLayer::ImGuiLayer()
        : Layer("ImGuiLayer"){

    }

    ImGuiLayer::~ImGuiLayer(){

    }

    void ImGuiLayer::OnUpdate(){
        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2(app.GetWindow().GetWidth(),app.GetWindow().GetHeight());
        
        float time = (float) glfwGetTime();
        io.DeltaTime = m_time > 0.0 ? (time - m_time) : (1.0f/60.0f);
        m_time = time;
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        static bool show = true;
        ImGui::ShowDemoWindow(&show);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    }
    void ImGuiLayer::OnEvent(Event& event){
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(SCIVIBE_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(SCIVIBE_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
        dispatcher.Dispatch<MouseMovedEvent>(SCIVIBE_BIND_EVENT_FN(ImGuiLayer::OnMouseMovedEvent));
        dispatcher.Dispatch<MouseScrolledEvent>(SCIVIBE_BIND_EVENT_FN(ImGuiLayer::OnMouseScrolledEvent));
        dispatcher.Dispatch<KeyPressedEvent>(SCIVIBE_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
        dispatcher.Dispatch<KeyReleasedEvent>(SCIVIBE_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
        dispatcher.Dispatch<WindowResizeEvent>(SCIVIBE_BIND_EVENT_FN(ImGuiLayer::OnWindowResizeEvent));
        dispatcher.Dispatch<KeyTypedEvent>(SCIVIBE_BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));


    }
    bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent &event){
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDown[event.GetMouseButton()] = true;
        return false;
    }
    bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent &event){
        ImGuiIO& io = ImGui::GetIO();
        io.MouseDown[event.GetMouseButton()] = false;
        return false; 
    }
    bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent &event){
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(event.GetX(),event.GetY());
        return false;
    }
    bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent &event){
        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheelH += event.GetXOffset();
        io.MouseWheel += event.GetYOffset();
        return false;
    }
    bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent &event){
        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[event.GetKeyCode()] = true;
        io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
        io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
        return false;

    }
    bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent &event){
        ImGuiIO& io = ImGui::GetIO();
        io.KeysDown[event.GetKeyCode()] = false;
        return false;

    }
    bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent &event){
        ImGuiIO& io = ImGui::GetIO();
        int c = event.GetKeyCode();
        if(c > 0 && c < 0x100000) io.AddInputCharacter((unsigned int short) c );


        return false;
    }
    bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent &event)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float) event.GetWidth(),(float) event.GetHeight());
        io.DisplayFramebufferScale = ImVec2(1.0f,1.0f);
        glViewport(0,0,event.GetWidth(), event.GetHeight());
        return false;
    }
    void ImGuiLayer::OnAttach()
    {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags != ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags != ImGuiBackendFlags_HasSetMousePos;
        // ici à remplacer
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Delete]    = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_LeftArrow]  = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_Home]       = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End]        = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Enter]      = GLFW_KEY_ENTER;
        ImGui_ImplOpenGL3_Init("#version 410");
    }
    void ImGuiLayer::OnDetach(){}

}