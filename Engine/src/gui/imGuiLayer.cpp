#include "pch/pch.hpp"
#include "core/core.hpp"
#include "imGuiLayer.hpp"
#include "imgui.h"
#include "core/application.hpp"

#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "ImGuizmo.h"

// glat and glfw does not need to be included ici
namespace scivibe{
    ImGuiLayer::ImGuiLayer()
        : Layer("ImGuiLayer"){
    }

    ImGuiLayer::~ImGuiLayer(){
    }

    void ImGuiLayer::OnAttach()
    {   
        IMGUI_CHECKVERSION() ;
        ImGui::CreateContext();
        
        ImGuiIO& io = ImGui::GetIO(); (void ) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        //io.Fonts->AddFontFromFileTTF("fonts/opensans/OpenSans-Bold.ttf", 18.0f);
        io.Fonts->AddFontFromFileTTF( FONT_ENGINE_PATH "Open_Sans/OpenSans-Bold.ttf", 24.0f);
        io.FontDefault = io.Fonts->AddFontFromFileTTF( FONT_ENGINE_PATH "Open_Sans/OpenSans-Regular.ttf", 24.0f);
        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable){
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        SetDarkThemeColor();
        Application& app =  Application::Get();
        GLFWwindow* Window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow()); 
        ImGui_ImplGlfw_InitForOpenGL(Window,true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }
    void ImGuiLayer::OnDetach(){
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    void ImGuiLayer::OnEvent(Event& e){
        if(m_BlockEvents){
            ImGuiIO& io = ImGui::GetIO();
            e.SetHandled( e.GetHandled() || (e.IsInCategory(EventCategoryMouse) && io.WantCaptureMouse) );
            e.SetHandled( e.GetHandled() || (e.IsInCategory(EventCategoryMouseButton) && io.WantCaptureMouse) );
            e.SetHandled( e.GetHandled() || (e.IsInCategory(EventCategoryKeyboard) && io.WantCaptureKeyboard) );
        }
    }

    void ImGuiLayer::Begin(){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }
    void ImGuiLayer::End(){
        ImGuiIO& io = ImGui::GetIO(); (void ) io;
        Application& app = Application::Get();
        io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable){
            GLFWwindow* context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(context);
        }
    }
    void ImGuiLayer::OnImGuiRender(){
    }
    void ImGuiLayer::SetDarkThemeColor(){
        auto& colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4{0.1f,0.105f,0.11f,1.0f};
        // Headers
        colors[ImGuiCol_Header] = ImVec4{0.2f,0.205f,0.21f,1.0f};
        colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f,0.305f,0.31f,1.0f};
        colors[ImGuiCol_HeaderActive] = ImVec4{0.15f,0.1505f,0.151f,1.0f};
        // button
        colors[ImGuiCol_Button] = ImVec4{0.2f,0.205f,0.21f,1.0f};
        colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f,0.305f,0.31f,1.0f};
        colors[ImGuiCol_ButtonActive] = ImVec4{0.15f,0.1505f,0.151f,1.0f};
        colors[ImGuiCol_FrameBg] = ImVec4{0.2f,0.205f,0.21f,1.0f};
        colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f,0.305f,0.31f,1.0f};
        colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f,0.1505f,0.151f,1.0f};
        // Tabs
        colors[ImGuiCol_Tab] = ImVec4{0.15f,0.1505f,0.151f,1.0f};
        colors[ImGuiCol_TabHovered] = ImVec4{0.38f,0.3805f,0.381f,1.0f};
        colors[ImGuiCol_TabActive] = ImVec4{0.28f,0.2805f,0.281f,1.0f};
        colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f,0.1505f,0.151f,1.0f};
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.28f,0.2805f,0.281f,1.0f};
        //tiltle
        colors[ImGuiCol_TitleBg] = ImVec4{0.15f,0.1505f,0.151f,1.0f};
        colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f,0.1505f,0.151f,1.0f};
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f,0.1505f,0.151f,1.0f};
    }

}
