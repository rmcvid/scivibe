#include "sandbox2D.hpp"
#include <exception>
#include <imgui.h>
#include "plateform/OpenGl/OpenGLShader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "renderer/renderer2D.hpp"
#include "renderer/rendererCurve2D.hpp"
#include <chrono>
#include <cfloat>
#include <cstdint>
#include <cmath>


Sandbox2D::Sandbox2D()
    :Layer("SandBox2D") , m_CameraController(1280.0f/720.0f, true)
{

}
void Sandbox2D::OnAttach(){
    // y'avait un m_FrameBuffer ici
    m_TextureTest = scivibe::Texture2D::Create(IMAGE_SANDBOX_PATH "chess.png");
    m_Font = scivibe::CreateRef<scivibe::Font>( FONT_SANDBOX_PATH "STIXTwoMath-Regular.ttf", 112 );
    scivibe::LatexSettings settings;
    settings.TextSize = 36.0f;
    settings.RasterSize = 112;
    settings.UnitsPerPixel = 0.005f;
    scivibe::PseudoLatex::Init(settings);
    m_PsFile.Load(TEXT_SANDBOX_PATH "test.ps");
    m_Line = scivibe::Line({-0.75,-0.75}, { 0.75,0.75},scivibe::Color::Green,10.0f);
    scivibe::Line destination = m_Line;
    destination.m_Position = {0.5f, 0.0f, 0.0f};
    destination.m_Rotation = 0.0f;
    destination.m_Scale = 1.5f;
    destination.m_Color = scivibe::Color::Red;
    m_LineAnimation.emplace(m_Line, destination, 2.0f);
    m_CurvePath.Contours.clear();
    m_CurvePath.Contours.emplace_back();
    auto& points = m_CurvePath.Contours.back().Points;
    constexpr int segmentCount = 200;
    points.reserve(segmentCount + 1);
    for (int i = 0; i <= segmentCount; ++i){
        const double t = static_cast<double>(i) / segmentCount;
        const double x = -1.0 + 2.0 * t;
        const double y = 0.5 * std::sin(6.0 * x);
        points.emplace_back(x, y);
    }
    m_CurveStyle.Color = {0.2f, 0.8f, 1.0f, 1.0f};
    m_CurveStyle.WidthPixels = 3.0f;
    m_CurveStyle.Join = scivibe::CurveJoin::Round;
    m_CurveStyle.Cap = scivibe::CurveCap::Round;
}
void Sandbox2D::OnDetach(){
    scivibe::PseudoLatex::Shutdown();
    m_Font.reset();
}
        
void Sandbox2D::OnUpdate(scivibe::Timestep ts ){
    scivibe::Renderer2D::ResetStats();
    auto& app = scivibe::Application::Get();
    const auto framebuffer = app.GetFrameBuffer();
    // Keep the scene at 1280 x 720; only its ImGui preview follows the panel size.
    const auto& renderSize = framebuffer->GetSpecification();
    // Window resize events concern the whole OS window, not this render target.
    m_CameraController.OnResize(static_cast<float>(renderSize.Width), static_cast<float>(renderSize.Height));
    m_CameraController.OnUpdate(ts);
    app.GetFrameBuffer()->Bind();
    //m_FrameBuffer->Bind();
    scivibe::RenderCommand::SetClearColor({0.0f,0.0f,0.0f,1.0f});
    scivibe::RenderCommand::Clear();
    scivibe::Renderer2D::BeginScene(m_CameraController.GetCamera());
    auto now = std::chrono::steady_clock::now();
    float time = std::chrono::duration<float>( now.time_since_epoch() ).count();
    //scivibe::Renderer2D::DrawQuad({1.0f, 1.0f, -0.1f},{std::sinf(time) * 1.0f, std::sinf(time)* 1.0f},time,m_TextureTest, 10.0f,{0.3f,0.3f,0.3f,1.0f});
    //scivibe::Renderer2D::DrawQuad({cos(time) * 1.0f, sin(time) * 1.0f, 0.0f},{std::sinf(time) * 1.0f, std::sinf(time)* 1.0f},time,{0.7f,0.7f,1.0f,1.0f});
    //scivibe::Renderer2D::DrawQuad({1.0f, 1.0f, -0.1f}, {1.0f,0.1f}, 0,{0.1f,0.4f,0.9f,1.0f});
    //scivibe::Renderer2D::DrawTriangle({0.0f,0.0f,0.0f}, {10.0f,0.0f,0.0f} , {5.0f,10.0f,0.0f}, m_TextureTest);
    //scivibe::Renderer2D::DrawArrow({0.0f,-1.0f,0.0f}, {1.0f,-1.0f,0.0f},{0.7f,0.7f,1.0f,1.0f});
    //scivibe::Renderer2D::DrawArrow({0.0f,1.0f,0.0f}, {1.0f,3.0f,0.0f},{0.7f,0.7f,1.0f,1.0f},10.f);
    //scivibe::RendererCurve2D::DrawCarthesian({0,0},{5,5},{1,1}, {0.25,0.25});
    //scivibe::RendererCurve2D::DrawPath(m_CurvePath, m_CurveStyle);
    if (m_LineAnimation) m_LineAnimation->Update();
    //scivibe::RendererCurve2D::DrawLine(m_LineAnimation);
    scivibe::RendererCurve2D::DrawLine(m_Line);
    scivibe::Renderer2D::EndScene();
    scivibe::Renderer2D::BeginScene(m_CameraController.GetCamera());
    scivibe::RenderCommand::SetDepthWrite(false);
    m_PsFile.Update(TEXT_SANDBOX_PATH "test.ps");
    if(m_PsFile.GetParagraphCount() > 0){
        scivibe::PseudoLatex::Write(m_PsFile.GetParagraph(), {-1.0f, 0.6f, 0.5f});
    }
    scivibe::Renderer2D::EndScene();
    scivibe::RenderCommand::SetDepthWrite(true);
    app.GetFrameBuffer()->Unbind();
    //m_FrameBuffer->Unbind();
}
void Sandbox2D::OnImGuiRender(){
    ImGui::SetNextWindowSizeConstraints(ImVec2(400.0f, 360.0f), ImVec2(FLT_MAX, FLT_MAX));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    
    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    if(opt_fullscreen){
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus; 
    }
    if(dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode){
        window_flags |= ImGuiWindowFlags_NoBackground;
    }
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f,0.0f));
    ImGui::Begin("Dockspace demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();
    if(opt_fullscreen){ ImGui::PopStyleVar(2); }
    ImGuiIO& io = ImGui::GetIO();
    if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable){
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f,0.0f), dockspace_flags);
    }
    if(ImGui::BeginMenuBar()){
        if(ImGui::BeginMenu("File")){
            if(ImGui::MenuItem("Exit")) scivibe::Application::Get().Close();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
    

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0,0});
    const bool viewportVisible = ImGui::Begin("ViewPort", nullptr,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    const ImVec2 viewportPanelSize = viewportVisible ? ImGui::GetContentRegionAvail() : ImVec2{0, 0};
    m_ViewPortSize = {std::max(0.0f, viewportPanelSize.x), std::max(0.0f, viewportPanelSize.y)};
    const auto framebuffer = scivibe::Application::Get().GetFrameBuffer();
    const auto& spec = framebuffer->GetSpecification();
    if (m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f && spec.Width > 0 && spec.Height > 0) {
        // Show precisely the recorded image, with margins instead of stretching it.
        const float scale = std::min(m_ViewPortSize.x / spec.Width, m_ViewPortSize.y / spec.Height);
        const ImVec2 imageSize{spec.Width * scale, spec.Height * scale};
        const ImVec2 origin = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2{
            origin.x + (m_ViewPortSize.x - imageSize.x) * 0.5f,
            origin.y + (m_ViewPortSize.y - imageSize.y) * 0.5f
        });
        const ImTextureID textureID = reinterpret_cast<ImTextureID>(
            static_cast<std::uintptr_t>(framebuffer->GetColorAttachmentRendererID()));
        ImGui::Image(textureID, imageSize, ImVec2{0,1}, ImVec2{1,0});
    }
    ImGui::PopStyleVar();
    ImGui::End();



    ImGui::Begin("Settings");
    auto stats = scivibe::Renderer2D::GetStats();
    ImGui::Text("Renderer2D stats:");
    ImGui::Text("DrawCalls %d",stats.DrawCalls);
    ImGui::Text("Quads %d: ",stats.QuadCount);
    ImGui::Text("Vertices %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices %d", stats.GetTotalIndexCount());

    ImGui::Separator();
    ImGui::BeginDisabled(!m_LineAnimation.has_value());
    if (ImGui::Button("Reset animation") && m_LineAnimation)
        m_LineAnimation->Reset();
    ImGui::EndDisabled();


    ImGui::Separator();
    const size_t paragraphCount = m_PsFile.GetParagraphCount();
    ImGui::BeginDisabled(paragraphCount == 0 || m_PsFile.GetParagraphIndex() == 0);
    if(ImGui::Button("Precedent")) m_PsFile.PreviousParagraph();
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(paragraphCount == 0 || m_PsFile.GetParagraphIndex() >= paragraphCount - 1);
    if(ImGui::Button("Suivant")) m_PsFile.NextParagraph();
    ImGui::EndDisabled();
    ImGui::SameLine();
    if(paragraphCount > 0){
        ImGui::Text("Paragraphe %zu / %zu", m_PsFile.GetParagraphIndex() + 1, paragraphCount);
    }else{
        ImGui::TextUnformatted("Aucun paragraphe");
    }


    ImGui::Separator();

    ImGui::Text("Frame timing");
    auto& app = scivibe::Application::Get();
    const auto deltaTime = scivibe::Application::GetDeltaTime();
    ImGui::Text("FPS (average): %d", app.GetFPS());
    ImGui::Text("Frame (average): %.2f ms", app.GetAverageFrameMilliseconds());
    ImGui::Text("Delta time: %.2f ms", deltaTime.GetMiliseconds());
    int targetFPS = app.GetTargetFPS();
    if (ImGui::InputInt("Target FPS", &targetFPS, 10, 60)) app.SetTargetFPS(targetFPS);
    ImGui::TextDisabled("0 = no software limit");
    bool vsync = app.GetWindow().IsVSync();
    if (ImGui::Checkbox("VSync", &vsync)) app.GetWindow().SetVSync(vsync);
    if (vsync) ImGui::TextDisabled("VSync also limits FPS to the display refresh rate.");
    ImGui::ColorEdit4("square Color", glm::value_ptr(m_Color));
    ImGui::Separator();
    const auto& imageSpec = app.GetFrameBuffer()->GetSpecification();
    ImGui::Text("Image : %u x %u px", imageSpec.Width, imageSpec.Height);
    ImGui::TextDisabled("Format video fixe : 16:9.");
    try {
        if (app.IsRecording()) {
            ImGui::TextUnformatted("Enregistrement en cours");
            if (ImGui::Button("Stop")) {app.StopRecording();}
        } else if (app.IsRecorderInitialized()) {
            ImGui::TextUnformatted("Pret a enregistrer");
            if (ImGui::Button("Start")) {app.StartRecording();}
            ImGui::SameLine();
            if (ImGui::Button("Annuler")) app.StopRecording();
        } else {
            ImGui::TextUnformatted("Enregistrement non initialise");
            if (ImGui::Button("Init")) app.InitRecording(VIDEO_SANDBOX_PATH "capture.mp4",60);
        } 
    }catch (const std::exception& error) {
        SCIVIBE_ERROR("Enregistrement : {}", error.what());
    }
    ImGui::End();

};
void Sandbox2D::OnEvent(scivibe::Event& e){
    m_CameraController.OnEvent(e);
};
