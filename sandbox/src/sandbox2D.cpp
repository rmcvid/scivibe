#include "sandbox2D.hpp"
#include <imgui.h>
#include "plateform/OpenGl/OpenGLShader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "renderer/renderer2D.hpp"
#include <chrono>


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
    ImGui::Begin("Settings");
    auto stats = scivibe::Renderer2D::GetStats();
    ImGui::Text("Renderer2D stats:");
    ImGui::Text("DrawCalls %d",stats.DrawCalls);
    ImGui::Text("Quads %d: ",stats.QuadCount);
    ImGui::Text("Vertices %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices %d", stats.GetTotalIndexCount());

    ImGui::Text("General informations:");
    const auto deltaTime = scivibe::Application::GetDeltaTime();
    const int fps = static_cast<int>(ImGui::GetIO().Framerate);
    ImGui::Text("Fps : %d", fps );

    
    ImGui::ColorEdit4("square Color", glm::value_ptr(m_Color));
    ImGui::End();

};
void Sandbox2D::OnEvent(scivibe::Event& e){
    m_CameraController.OnEvent(e);
};
