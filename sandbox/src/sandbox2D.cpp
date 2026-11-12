#include "sandbox2D.hpp"
#include <exception>
#include <algorithm>
#include <imgui.h>
#include <imgui_internal.h>
#include "plateform/OpenGl/OpenGLShader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "renderer/renderer2D.hpp"
#include "renderer/rendererCurve2D.hpp"
#include <chrono>
#include <cfloat>
#include <cstdint>
#include <cmath>
#include "ImGuizmo.h"




Sandbox2D::Sandbox2D()
    :Layer("SandBox2D") , m_CameraController(1280.0f/720.0f, true)
{

}
void Sandbox2D::OnAttach(){
    // y'avait un m_FrameBuffer ici
    m_TextureTest = scivibe::Texture2D::Create(IMAGE_SANDBOX_PATH "chess.png");
    m_Font = scivibe::CreateRef<scivibe::Font>( FONT_SANDBOX_PATH "STIXTwoMath-Regular.ttf", 112 );
    scivibe::LatexSettings settings;
    settings.TextSize = 62.0f;
    settings.RasterSize = 112;
    settings.UnitsPerPixel = 0.005f;
    scivibe::PseudoLatex::Init(settings);
    m_PsFile.Load(TEXT_SANDBOX_PATH "test.ps");
    m_ActiveScene = scivibe::CreateRef<scivibe::Scene>();
    const auto& renderSize = scivibe::Application::Get().GetFrameBuffer()->GetSpecification();
    m_EditorCamera = scivibe::EditorCamera(30.0f,
        static_cast<float>(renderSize.Width) / renderSize.Height, 0.1f, 1000.0f);
    m_EditorCamera.SetViewportSize(renderSize.Width, renderSize.Height);
    m_ActiveScene->OnViewPortResize(renderSize.Width, renderSize.Height);

    #if 0
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

    auto square = m_ActiveScene->CreateEntity("square");
    square.AddComponent<scivibe::SpriteRendererComponent>(glm::vec4 {0.0f,1.0f,0.0f,1.0f});
    m_SquareEntity = square;
    m_CameraEntity = m_ActiveScene->CreateEntity("camera");
    m_CameraEntity.AddComponent<scivibe::CameraComponent>();
    m_SecondCamera = m_ActiveScene->CreateEntity("SecondCamera");
    
   
    class CameraController : public scivibe::ScriptableEntity{
    public :
        void OnCreate()  override  {
            
        }
        void OnUpdate(scivibe::Timestep ts) override {
            auto& translation = GetComponent<scivibe::TransformComponent>().Translation;
            const float step = 5.0f * static_cast<float>(ts.GetSeconds());
            if (scivibe::Input::IsKeyPressed(SCIVIBE_KEY_Q))
                translation.x -= step;
            if (scivibe::Input::IsKeyPressed(SCIVIBE_KEY_D))
                translation.x += step;
            if (scivibe::Input::IsKeyPressed(SCIVIBE_KEY_Z))
                translation.y += step;
            if (scivibe::Input::IsKeyPressed(SCIVIBE_KEY_S))
                translation.y -= step;
        }
        void OnDestroy()  override {
            
        }
    };
    m_CameraEntity.AddComponent<scivibe::NativeScriptComponent>().Bind<CameraController>();
#endif

    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}
void Sandbox2D::OnDetach(){
    scivibe::PseudoLatex::Shutdown();
    m_Font.reset();
}
        
void Sandbox2D::OnUpdate(scivibe::Timestep ts ){
    scivibe::Renderer2D::ResetStats();
    auto& app = scivibe::Application::Get();
    
    const auto framebuffer = app.GetFrameBuffer();
    const auto& renderSize = framebuffer->GetSpecification();
    // The panel only scales the preview; it never changes the captured image size.
    if (!app.IsRecorderInitialized() &&
        (renderSize.Width != m_RenderResolution.x || renderSize.Height != m_RenderResolution.y)) {
        framebuffer->Resize(m_RenderResolution.x, m_RenderResolution.y);
        m_EditorCamera.SetViewportSize(renderSize.Width, renderSize.Height);
        m_ActiveScene->OnViewPortResize(renderSize.Width, renderSize.Height);
    }
    if (m_ResizeViewportRequested) {
        ResizeViewportToRecording();
        m_ResizeViewportRequested = false;
    }
    // Middle-button navigation takes precedence over manipulating an entity.
    ImGuizmo::Enable(!scivibe::Input::IsMouseButtonPressed(SCIVIBE_MOUSE_BUTTON_MIDDLE));
    m_EditorCamera.OnUpdate(ts,
        m_ViewPortHovered && !ImGuizmo::IsUsing() && !ImGui::GetIO().WantTextInput);

    app.GetFrameBuffer()->Bind();
    scivibe::RenderCommand::SetClearColor({0.0f,0.0f,0.0f,1.0f});
    scivibe::RenderCommand::Clear();
    m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
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
    //scivibe::RendererCurve2D::DrawLine(m_LineAnimation);
    //if (m_LineAnimation) m_LineAnimation->Update();
    //scivibe::RendererCurve2D::DrawLine(m_Line);
    //scivibe::Renderer2D::EndScene();
    //scivibe::Renderer2D::BeginScene(m_CameraController.GetCamera());
    //scivibe::RenderCommand::SetDepthWrite(false);

    //m_PsFile.Update(TEXT_SANDBOX_PATH "test.ps");
    //if(m_PsFile.GetParagraphCount() > 0){
    //    scivibe::PseudoLatex::Write(m_PsFile.GetParagraph(), {-1.0f, 0.6f, 0.5f});
    //}
    //scivibe::Renderer2D::EndScene();
    scivibe::RenderCommand::SetDepthWrite(true);
    app.GetFrameBuffer()->Unbind();
}

void Sandbox2D::ResizeViewportToRecording() {
    ImGuiWindow* viewportWindow = ImGui::FindWindowByName("ViewPort");
    if (!viewportWindow || m_ViewPortSize.x <= 0.0f || m_ViewPortSize.y <= 0.0f)
        return;

    const auto& spec = scivibe::Application::Get().GetFrameBuffer()->GetSpecification();
    const ImVec2 pixelScale = ImGui::GetIO().DisplayFramebufferScale;
    const ImVec2 delta{
        spec.Width / std::max(pixelScale.x, 0.01f) - m_ViewPortSize.x,
        spec.Height / std::max(pixelScale.y, 0.01f) - m_ViewPortSize.y
    };

    ImGuiDockNode* root = viewportWindow->DockNode;
    if (!root) {
        // A floating panel needs its title bar and borders in addition to the image.
        ImGui::SetWindowSize(viewportWindow, ImVec2{
            viewportWindow->SizeFull.x + delta.x, viewportWindow->SizeFull.y + delta.y});
        return;
    }

    // Give the size change to the viewport branch, preserving neighbouring panels.
    // ImGui normally distributes resizing between dock nodes according to ratios.
    while (root->ParentNode) {
        ImGuiDockNode* parent = root->ParentNode;
        ImGuiDockNode* sibling = parent->ChildNodes[parent->ChildNodes[0] == root ? 1 : 0];
        root->WantLockSizeOnce = false;
        sibling->WantLockSizeOnce = true;
        root = parent;
    }

    if (root->IsDockSpace() && root->HostWindow &&
        root->HostWindow->Viewport == ImGui::GetMainViewport()) {
        auto& window = scivibe::Application::Get().GetWindow();
        window.SetSize(
            static_cast<uint32_t>(std::max(1.0f, std::round(window.GetWidth() + delta.x))),
            static_cast<uint32_t>(std::max(1.0f, std::round(window.GetHeight() + delta.y))));
    } else {
        // A detached dock group owns a separate ImGui platform window.
        ImGui::DockBuilderSetNodeSize(root->ID, ImVec2{
            root->Size.x + delta.x, root->Size.y + delta.y});
    }
}
void Sandbox2D::ImportText() {
    scivibe::PsFile file;
    if (!file.Load(TEXT_SANDBOX_PATH "test.ps"))
        return;
    for (size_t i = 0; i < file.GetParagraphCount(); ++i) {
        const auto& source = file.GetParagraph(i);
        if (source.empty())continue;
        auto entity = m_ActiveScene->CreateEntity( "Texte " + std::to_string(i + 1));
        entity.AddComponent<scivibe::TextComponent>().Source = source;
        auto& transform = entity.GetComponent<scivibe::TransformComponent>();
        transform.Translation = {-3.0f,2.0f - static_cast<float>(i) * 1.5f,0.0f};
    }
}

void Sandbox2D::OnImGuiRender(){
    // ImGui receives keys even while the engine layer blocks editor events.
    // Leave arrow keys to text fields and active numeric controls while editing.
    if (!ImGui::GetIO().WantTextInput && !ImGui::IsAnyItemActive() && !ImGuizmo::IsUsing()) {
        if (ImGui::IsKeyPressed(ImGuiKey_RightArrow, false)) m_ActiveScene->SetCurrentSlide(m_ActiveScene->GetCurrentSlide() + 1);
        if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow, false)) m_ActiveScene->SetCurrentSlide(m_ActiveScene->GetCurrentSlide() - 1);
    }
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
    ImGuiStyle& style = ImGui::GetStyle();
    float windowMinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable){
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f,0.0f), dockspace_flags);
    }
    style.WindowMinSize.x = windowMinSizeX;
    if(ImGui::BeginMenuBar()){
        if(ImGui::BeginMenu("File")){
            if( ImGui::MenuItem("New", "Ctrl+N")){
                NewScene();
            }
            if( ImGui::MenuItem("Save as ...", "Ctrl+Shift+S")){
                SaveSceneAs();
            }
            if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                OpenScene();
            }
             if (ImGui::MenuItem("Importer les textes")) {
                ImportText();
            }
            if(ImGui::MenuItem("Exit")) scivibe::Application::Get().Close();
            ImGui::EndMenu();
           
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
    

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0,0});
    const bool viewportVisible = ImGui::Begin("ViewPort", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    m_ViewPortFocused = ImGui::IsWindowFocused();
    m_ViewPortHovered = false;
    const ImVec2 viewportPanelSize = viewportVisible ? ImGui::GetContentRegionAvail() : ImVec2{0, 0};
    m_ViewPortSize = {std::max(0.0f, viewportPanelSize.x), std::max(0.0f, viewportPanelSize.y)};
    const auto framebuffer = scivibe::Application::Get().GetFrameBuffer();
    const auto& spec = framebuffer->GetSpecification();
    ImVec2 imagePosition{0.0f, 0.0f};
    ImVec2 imageSize{0.0f, 0.0f};
    if (m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f && spec.Width > 0 && spec.Height > 0) {
        // Show precisely the recorded image, with margins instead of stretching it.
        const float scale = std::min(m_ViewPortSize.x / spec.Width, m_ViewPortSize.y / spec.Height);
        imageSize = ImVec2{spec.Width * scale, spec.Height * scale};
        const ImVec2 origin = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2{
            origin.x + (m_ViewPortSize.x - imageSize.x) * 0.5f,
            origin.y + (m_ViewPortSize.y - imageSize.y) * 0.5f
        });
        const ImTextureID textureID = reinterpret_cast<ImTextureID>(
            static_cast<std::uintptr_t>(framebuffer->GetColorAttachmentRendererID()));
        imagePosition = ImGui::GetCursorScreenPos();
        ImGui::Image(textureID, imageSize, ImVec2{0,1}, ImVec2{1,0});
        m_ViewPortHovered = ImGui::IsItemHovered();
    }
    scivibe::Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewPortFocused && !m_ViewPortHovered);
    //Guizmos
    scivibe::Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
    if(selectedEntity && !m_ActiveScene->HasAnimatedPose(selectedEntity) && m_GuizmoType != -1 && imageSize.x > 0.0f && imageSize.y > 0.0f &&
       !scivibe::Input::IsMouseButtonPressed(SCIVIBE_MOUSE_BUTTON_MIDDLE)){

        const glm::mat4 cameraProjection = m_EditorCamera.GetProjection();
        glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(imagePosition.x, imagePosition.y, imageSize.x, imageSize.y);
        
        //entity
        auto& tc = selectedEntity.GetComponent<scivibe::TransformComponent>();
        glm::mat4 transform = tc.GetTransform();
        
        //snapping
        bool snap = scivibe::Input::IsKeyPressed(SCIVIBE_KEY_LEFT_CONTROL);
        float snapValue = 0.5f; //snap to 0.5m
        if(m_GuizmoType == ImGuizmo::OPERATION::ROTATE) snapValue = 45.0f;
        
        float snapValues[3] = {snapValue,snapValue,snapValue};
        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), 
            (ImGuizmo::OPERATION)m_GuizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform), 
            nullptr, snap ? snapValues : nullptr);

        if(ImGuizmo::IsUsing()){
            glm::vec3 translation, rotation, scale; 
            if (scivibe::Math::DecomposeTransform(transform,translation, rotation, scale)) {
                tc.Translation = translation;
                tc.Rotation = rotation;
                tc.Scale = scale;
            }
        }

    }

    ImGui::PopStyleVar();
    ImGui::End();

    m_SceneHierarchyPanel.OnImGuiRender();

    ImGui::Begin("Settings");
    int slide = m_ActiveScene->GetCurrentSlide();
    ImGui::Text("Slide %d", slide);
    ImGui::BeginDisabled(slide <= 1);
    if (ImGui::Button("< Precedente")) m_ActiveScene->SetCurrentSlide(slide - 1);
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button("Suivante >")) m_ActiveScene->SetCurrentSlide(slide + 1);
    slide = m_ActiveScene->GetCurrentSlide();
    if (ImGui::InputInt("Numero de slide", &slide)) m_ActiveScene->SetCurrentSlide(slide);
    bool animateSlides = m_ActiveScene->GetAnimateSlides();
    if (ImGui::Checkbox("Apercu des animations", &animateSlides)) m_ActiveScene->SetAnimateSlides(animateSlides);
    if (ImGui::Button("Rejouer la slide")) m_ActiveScene->RestartSlideAnimations();
    if (ImGui::Button("Reinitialiser les valeurs animees")) m_ActiveScene->ResetAnimationPreview();
    ImGui::TextWrapped("Desactiver l'apercu pour modifier les transformations avec les gizmos.");
    ImGui::Separator();
    auto stats = scivibe::Renderer2D::GetStats();
    ImGui::Text("Renderer2D stats:");
    ImGui::Text("DrawCalls %d",stats.DrawCalls);
    ImGui::Text("Quads %d: ",stats.QuadCount);
    ImGui::Text("Vertices %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices %d", stats.GetTotalIndexCount());
#if 0
// ici il faudrait remettre ces animations et ce suivit d'affichage dans le panneau du imgui
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
#endif
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
    ImGui::Separator();
    const auto& imageSpec = app.GetFrameBuffer()->GetSpecification();
    ImGui::Text("Image : %u x %u px", imageSpec.Width, imageSpec.Height);
    ImGui::Text("Zone viewport : %.0f x %.0f px",
        m_ViewPortSize.x * ImGui::GetIO().DisplayFramebufferScale.x,
        m_ViewPortSize.y * ImGui::GetIO().DisplayFramebufferScale.y);
    ImGui::TextDisabled("Format video fixe : 16:9.");
    ImGui::BeginDisabled(app.IsRecorderInitialized());
    int resolutionIndex = m_RenderResolution.x == 1920 ? 1 : m_RenderResolution.x == 2560 ? 2 : 0;
    const char* resolutions[] = {"1280 x 720", "1920 x 1080", "2560 x 1440"};
    if (ImGui::Combo("Resolution video", &resolutionIndex, resolutions, 3)) {
        const glm::uvec2 sizes[] = {{1280, 720}, {1920, 1080}, {2560, 1440}};
        m_RenderResolution = sizes[resolutionIndex];
    }
    ImGui::EndDisabled();
    if (ImGui::Button("Resize viewport")){
        m_ResizeViewportRequested = true;
    }

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
            // Apply any new resolution in OnUpdate before initializing the encoder.
            ImGui::BeginDisabled(imageSpec.Width != m_RenderResolution.x || imageSpec.Height != m_RenderResolution.y);
            const bool initializeRecording = ImGui::Button("Init");
            ImGui::EndDisabled();
            if (initializeRecording) app.InitRecording(VIDEO_SANDBOX_PATH "capture.mp4",60);
        } 
    }catch (const std::exception& error) {
        SCIVIBE_ERROR("Enregistrement : {}", error.what());
    }
    ImGui::End();

};
void Sandbox2D::NewScene(){
    m_ActiveScene = scivibe::CreateRef<scivibe::Scene>();
    const auto& size = scivibe::Application::Get().GetFrameBuffer()->GetSpecification();
    m_ActiveScene->OnViewPortResize(size.Width, size.Height);
    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}
void Sandbox2D::SaveSceneAs(){
    std::string filepath = scivibe::FileDialogs::SaveFile("Scivibe scene (*.scb)\0*.scivibe\0;*.yaml\0");
    if(!filepath.empty()){
        scivibe::SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(filepath);
    }
}
void Sandbox2D::OpenScene(){
    std::string filepath = scivibe::FileDialogs::OpenFile("Scivibe Scene (*.scb;*.scivibe;*.yaml)\0*.scb;*.scivibe;*.yaml\0");
    if (!filepath.empty()) {
        auto loadedScene = scivibe::CreateRef<scivibe::Scene>();
        const auto& size = scivibe::Application::Get().GetFrameBuffer()->GetSpecification();
        loadedScene->OnViewPortResize(size.Width, size.Height);
        scivibe::SceneSerializer serializer(loadedScene);
        if (serializer.Deserialize(filepath)) {
            m_ActiveScene = loadedScene;
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        } else {
            SCIVIBE_ERROR("Impossible de charger : {}", filepath);
        }
    }
}

bool Sandbox2D::OnKeyPressed(scivibe::KeyPressedEvent& e){
    if(e.GetRepeatCount() > 0 || ImGui::GetIO().WantTextInput) return false;
    bool control = scivibe::Input::IsKeyPressed(SCIVIBE_KEY_LEFT_CONTROL) || scivibe::Input::IsKeyPressed(SCIVIBE_KEY_RIGHT_CONTROL);
    bool shift = scivibe::Input::IsKeyPressed(SCIVIBE_KEY_LEFT_SHIFT) || scivibe::Input::IsKeyPressed(SCIVIBE_KEY_RIGHT_SHIFT);
    switch (e.GetKeyCode()){
        case SCIVIBE_KEY_N:{
            if(control) NewScene();
            return true;
        }
        case SCIVIBE_KEY_O :{
            if(control) OpenScene();
            return true;

        }
        case SCIVIBE_KEY_S:{
            if(control && shift) SaveSceneAs();
            return true;
        }
        case SCIVIBE_KEY_Q :
            m_GuizmoType = -1;
            return true;
        case SCIVIBE_KEY_W :
            m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
            return true;
        case SCIVIBE_KEY_E :
            m_GuizmoType = ImGuizmo::OPERATION::ROTATE;
            return true;
        case SCIVIBE_KEY_R :
            m_GuizmoType = ImGuizmo::OPERATION::SCALE;
            return true;
        case SCIVIBE_KEY_SPACE:
            if (!m_ViewPortFocused) return false;
            m_ResizeViewportRequested = true;
            return true;
        default:
            return false;
    }
}
void Sandbox2D::OnEvent(scivibe::Event& e){
    if (m_ViewPortHovered && !ImGuizmo::IsUsing() && !ImGui::GetIO().WantTextInput)
        m_EditorCamera.OnEvent(e);
    scivibe::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<scivibe::KeyPressedEvent>(SCIVIBE_BIND_EVENT_FN(Sandbox2D::OnKeyPressed));
};
