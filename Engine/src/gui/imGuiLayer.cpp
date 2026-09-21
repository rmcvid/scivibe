#include "pch/pch.hpp"
#include "core/core.hpp"
#include "imGuiLayer.hpp"
#include "imgui.h"
#include "application/application.hpp"

#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

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

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable){
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
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

    void ImGuiLayer::Begin(){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
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
        static bool show = true;
        ImGui::ShowDemoWindow(&show);
    }

}
