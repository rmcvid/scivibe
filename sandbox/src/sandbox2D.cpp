#include "sandbox2D.hpp"
#include <exception>
#include <imgui.h>
#include "plateform/OpenGl/OpenGLShader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "renderer/renderer2D.hpp"
#include <chrono>
#include <cfloat>


Sandbox2D::Sandbox2D()
    :Layer("SandBox2D") , m_CameraController(1280.0f/720.0f, true)
{

}
void Sandbox2D::OnAttach(){
    m_TextureTest = scivibe::Texture2D::Create(IMAGE_SANDBOX_PATH "chess.png");
    
}
void Sandbox2D::OnDetach(){

}
        
void Sandbox2D::OnUpdate(scivibe::Timestep ts ){

    m_CameraController.OnUpdate(ts);

    scivibe::RenderCommand::SetClearColor({0.0f,0.0f,0.0f,0.0f});
    scivibe::RenderCommand::Clear();
    scivibe::Renderer2D::ResetStats();

    scivibe::Renderer2D::BeginScene(m_CameraController.GetCamera());
    auto now = std::chrono::steady_clock::now();
    float time = std::chrono::duration<float>( now.time_since_epoch() ).count();
    scivibe::Renderer2D::DrawQuad({1.0f, 1.0f, -0.1f},{std::sinf(time) * 1.0f, std::sinf(time)* 1.0f},time,m_TextureTest, 10.0f,{0.3f,0.3f,0.3f,1.0f});
    scivibe::Renderer2D::DrawQuad({cos(time) * 1.0f, sin(time) * 1.0f, 0.0f},{std::sinf(time) * 1.0f, std::sinf(time)* 1.0f},time,{0.7f,0.7f,1.0f,1.0f});
    
    scivibe::Renderer2D::EndScene();

    scivibe::Renderer2D::BeginScene(m_CameraController.GetCamera());
    for(float y = -0.5f;y<5.0f;y+=0.5f){
        for(float x = -0.5f;x<5.0f;x+=0.5f){
            glm::vec4 color {(x+5.0f)/10.0f,0.4f,(y+5.0f)/10.0f,0.5f };
            scivibe::Renderer2D::DrawQuad({x,y}, {0.45f,0.45f},color);
        }
    }
    scivibe::Renderer2D::EndScene();


}
void Sandbox2D::OnImGuiRender(){
    ImGui::SetNextWindowSizeConstraints(ImVec2(400.0f, 360.0f), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::Begin("Settings");
    auto stats = scivibe::Renderer2D::GetStats();
    ImGui::Text("Renderer2D stats:");
    ImGui::Text("DrawCalls %d",stats.DrawCalls);
    ImGui::Text("Quads %d: ",stats.QuadCount);
    ImGui::Text("Vertices %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices %d", stats.GetTotalIndexCount());

    ImGui::Separator();
    ImGui::Text("Frame timing");
    auto& app = scivibe::Application::Get();
    const auto deltaTime = scivibe::Application::GetDeltaTime();
    ImGui::Text("FPS (average): %d", app.GetFPS());
    ImGui::Text("Frame (average): %.2f ms", app.GetAverageFrameMilliseconds());
    ImGui::Text("Delta time: %.2f ms", deltaTime.GetMiliseconds());

    int targetFPS = app.GetTargetFPS();
    if (ImGui::InputInt("Target FPS", &targetFPS, 10, 60))
        app.SetTargetFPS(targetFPS);
    ImGui::TextDisabled("0 = no software limit");

    bool vsync = app.GetWindow().IsVSync();
    if (ImGui::Checkbox("VSync", &vsync))
        app.GetWindow().SetVSync(vsync);
    if (vsync)
        ImGui::TextDisabled("VSync also limits FPS to the display refresh rate.");

    
    ImGui::ColorEdit4("square Color", glm::value_ptr(m_Color));

    ImGui::Separator();
    try {
        if (!app.IsRecording()) {
            if (ImGui::Button("Demarrer l'enregistrement")) {
                scivibe::CapturedFrame frame;
                // Recupere les dimensions reelles du framebuffer.
                if (app.GetWindow().CaptureFrame(frame)) {
                    app.StartRecording( VIDEO_SANDBOX_PATH "capture.mp4", 60);
                } else {
                    SCIVIBE_ERROR("Impossible de capturer la fenetre");
                }
            }
        } else {
            ImGui::TextUnformatted("Enregistrement en cours");
            if (ImGui::Button("Arreter l'enregistrement")) app.StopRecording();
        }
    } catch (const std::exception& error) {
        SCIVIBE_ERROR("Enregistrement : {}", error.what());
    }
    ImGui::End();

};
void Sandbox2D::OnEvent(scivibe::Event& e){
    m_CameraController.OnEvent(e);
};
