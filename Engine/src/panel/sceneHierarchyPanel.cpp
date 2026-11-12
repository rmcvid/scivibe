#include "panel/sceneHierarchyPanel.hpp"
#include "scene/component.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include "glm/gtc/type_ptr.hpp"
#include <misc/cpp/imgui_stdlib.h>
#include "utils/platformUtils.hpp"
// ici réécrire de façon plus compacte
namespace scivibe {
    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context) {
        SetContext(context);
    }
    void SceneHierarchyPanel::SetContext(const Ref<Scene>& context) {
        m_Context = context;
        m_SelectionContext = Entity{};
    }
    void SceneHierarchyPanel::OnImGuiRender() {
        if (ImGui::Begin("Scene Hierarchy")) {
            if (m_Context) {
                auto view = m_Context->m_Registry.view<TagComponent>();
                for (auto entityID : view) {
                    DrawEntityNode(Entity{entityID, m_Context.get()});
                }
            }
        }
        if (ImGui::IsMouseClicked(0) &&ImGui::IsWindowHovered() &&!ImGui::IsAnyItemHovered()){
            m_SelectionContext = {};
        }

        // right click on blank space
        if (ImGui::BeginPopupContextWindow( "HierarchyContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (m_Context && ImGui::MenuItem("Create Empty Entity")) {
                m_Context->CreateEntity("Empty Entity");
            }
            if (m_Context && ImGui::MenuItem("Creer un repere cartesien")) {
                m_SelectionContext = m_Context->CreateEntity("Repere cartesien");
                m_SelectionContext.AddComponent<CartesianGridComponent>();
            }
            if (m_Context && ImGui::MenuItem("Creer une fonction")) {
                m_SelectionContext = m_Context->CreateEntity("Fonction");
                m_SelectionContext.AddComponent<FunctionCurveComponent>();
            }
            ImGui::EndPopup();
        }

        ImGui::End();
        ImGui::Begin("Properties");
        if(m_SelectionContext){
            DrawComponents(m_SelectionContext);
        }
        ImGui::End();

    }
    void SceneHierarchyPanel::DrawEntityNode(Entity entity){
        auto& tag = entity.GetComponent<TagComponent>().Tag;
        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity ) ?  ImGuiTreeNodeFlags_Selected : 0 ) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity,flags,"%s", tag.c_str());
        if(ImGui::IsItemClicked()){
            m_SelectionContext = entity;
        }
        bool entityDeleted = false;
        if(ImGui::BeginPopupContextItem()){
            if(ImGui::MenuItem("Delete Entity")){
                entityDeleted = true;
            }
            ImGui::EndPopup();
        }
        if(opened){
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
            bool opened = ImGui::TreeNodeEx((void*) 9817239, flags, "Child");
            if(opened){
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        if(entityDeleted){
            m_Context->DestroyEntity(entity);
            if(m_SelectionContext == entity) m_SelectionContext = {};
        }
    }
    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue =0.0f, float columnWidth = 100.0f){
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0,columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3,ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0,0});
        
        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y *2.0f;
        ImVec2 buttonSize = {lineHeight +3.0f, lineHeight};
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f,0.1f,0.15f,1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f,0.2f,0.2f,1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f,0.1f,0.15f,1.0f});
        ImGui::PushFont(boldFont);
        if(ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        
        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x,0.1f);
        ImGui::PopItemWidth();
        ImGui::SameLine();


        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f,0.7f,0.2f,1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f,0.8f,0.3f,1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f,0.7f,0.2f,1.0f});
        ImGui::PushFont(boldFont);
        if(ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y,0.1f);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f,0.25f,0.8f,1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f,0.35f,0.9f,1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f,0.25f,0.8f,1.0f});
        ImGui::PushFont(boldFont);
        if(ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z,0.1f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        
        ImGui::PopStyleVar();
        ImGui::Columns(1);
        ImGui::PopID();
    }
    static void DrawSlideList(std::vector<int>& slides, bool emptyMeansAll, int currentSlide) {
        ImGui::PushID(emptyMeansAll ? "VisibilitySlides" : "TriggerSlides");
        if (emptyMeansAll) {
            bool all = slides.empty();
            if (ImGui::Checkbox("Toutes les slides", &all)) {
                if (all) slides.clear();
                else slides.push_back(currentSlide);
            }
            if (all) { ImGui::PopID(); return; }
        }
        for (size_t i = 0; i < slides.size();) {
            ImGui::PushID(static_cast<int>(i));
            ImGui::SetNextItemWidth(110.0f);
            if (ImGui::InputInt("##Slide", &slides[i])) slides[i] = std::clamp(slides[i], 1, 1000000);
            ImGui::SameLine();
            const bool remove = ImGui::SmallButton("Supprimer");
            ImGui::PopID();
            if (remove) slides.erase(slides.begin() + i);
            else ++i;
        }
        if (ImGui::Button("Ajouter un numero")) {
            int next = slides.empty() ? currentSlide : std::min(slides.back() + 1, 1000000);
            slides.push_back(next);
        }
        if (!emptyMeansAll && slides.empty()) ImGui::TextDisabled("Aucun declenchement.");
        ImGui::PopID();
    }
    template<typename T, typename UIFunction>

    static void DrawComponent(const std::string& name
        , Entity entity, UIFunction uiFunction, int currentSlide = 1){
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        if(entity.HasComponent<T>()){
            ImGui::PushID(name.c_str());
            auto& component = entity.GetComponent<T>();
            ImVec2 contentRegionAvailable =  ImGui::GetContentRegionAvail();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4,4});
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y +2.0f;
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*) typeid(T).hash_code(), treeNodeFlags, name.c_str());
            ImGui::PopStyleVar();

            ImGui::SameLine(contentRegionAvailable.x - lineHeight*0.5f);
            if(ImGui::Button("+", ImVec2{lineHeight,lineHeight})){
                ImGui::OpenPopup("ComponentSettings");
            }
            
            bool removeComponent = false;
            if(ImGui::BeginPopup("ComponentSettings")){
                if(ImGui::MenuItem("Remove Component", nullptr, false, !std::is_same_v<T, TransformComponent>)){
                    removeComponent = true;
                }
                ImGui::EndPopup();
            }
            if(open && !removeComponent){
                ImGui::TreePush("Body");
                uiFunction(component);
                if constexpr (requires { component.Slides; }) {
                    ImGui::Separator();
                    ImGui::TextUnformatted("Afficher sur les slides");
                    DrawSlideList(component.Slides, true, currentSlide);
                }
                ImGui::TreePop();
            }
             if(removeComponent){
                entity.RemoveComponent<T>();
            }
            ImGui::PopID();
        }
    }
    static void DrawCurveStyle(CurveStyle& style) {
        ImGui::ColorEdit4("Couleur", glm::value_ptr(style.Color));
        ImGui::DragFloat("Epaisseur (px)", &style.WidthPixels, 0.1f, 0.1f, 100.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
        int join = static_cast<int>(style.Join), cap = static_cast<int>(style.Cap);
        if (ImGui::Combo("Raccords", &join, "Miter\0Bevel\0Round\0")) style.Join = static_cast<CurveJoin>(join);
        if (ImGui::Combo("Extremites", &cap, "Butt\0Square\0Round\0")) style.Cap = static_cast<CurveCap>(cap);
        if (style.Join == CurveJoin::Miter)
            ImGui::DragFloat("Limite Miter", &style.MiterLimit, 0.1f, 1.0f, 20.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
    }
    static void DrawColorTarget(const char* label, AnimationColorTarget& target) {
        int selected = static_cast<int>(target);
        if (ImGui::Combo(label, &selected, "Sprite\0Texte\0Repere\0Fonction\0"))
            target = static_cast<AnimationColorTarget>(selected);
    }
    static void DrawAnimationSource(const char* label, AnimationValueSource& source, Scene& scene, bool color) {
        ImGui::PushID(label);
        ImGui::Checkbox("Valeur d'une entite", &source.UseEntity);
        if (source.UseEntity) {
            auto& registry = *scene.Reg();
            const auto* tag = registry.valid(source.Entity) ? registry.try_get<TagComponent>(source.Entity) : nullptr;
            if (ImGui::BeginCombo("Entite source", tag ? tag->Tag.c_str() : "Choisir une entite")) {
                if (ImGui::Selectable("Aucune", source.Entity == entt::null)) source.Entity = entt::null;
                for (auto candidate : registry.view<TagComponent, TransformComponent>()) {
                    ImGui::PushID(static_cast<int>(static_cast<uint32_t>(candidate)));
                    const auto& name = registry.get<TagComponent>(candidate).Tag;
                    if (ImGui::Selectable(name.c_str(), source.Entity == candidate)) source.Entity = candidate;
                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }
            if (color) {
                DrawColorTarget("Composant source", source.ColorTarget);
                glm::vec4 ignored;
                if (!scene.TryGetRenderColor(source.Entity, source.ColorTarget, ignored))
                    ImGui::TextWrapped("La source ne possede pas cette couleur. La valeur manuelle servira de repli.");
            } else if (!registry.valid(source.Entity) || !registry.all_of<TransformComponent>(source.Entity)) {
                ImGui::TextWrapped("Source absente : la valeur manuelle servira de repli.");
            }
        }
        ImGui::PopID();
    }
    void SceneHierarchyPanel::DrawComponents(Entity entity){
        if(entity.HasComponent<TagComponent>()){
            auto& tag = entity.GetComponent<TagComponent>().Tag;
            char buffer[256];
            memset(buffer,0,sizeof(buffer));
            strcpy_s(buffer,sizeof(buffer), tag.c_str());
            if( ImGui::InputText("##Tag",buffer, sizeof(buffer))){
                tag = std::string(buffer);  
            }
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        if(ImGui::Button("Add Component")){
                ImGui::OpenPopup("AddComponent");
            }
        if(ImGui::BeginPopup("AddComponent")){
            if(!entity.HasComponent<CameraComponent>() && ImGui::MenuItem("Camera Component")){
                m_SelectionContext.AddComponent<CameraComponent>();
                ImGui::CloseCurrentPopup();
            }
            if(!entity.HasComponent<SpriteRendererComponent>() && ImGui::MenuItem("Sprite Renderer Component")){
                m_SelectionContext.AddComponent<SpriteRendererComponent>();
                ImGui::CloseCurrentPopup();
            }
            if (!entity.HasComponent<TextComponent>() &&ImGui::MenuItem("Text Component")){
                entity.AddComponent<TextComponent>().Source = "Texte";
                ImGui::CloseCurrentPopup();
            }
            if (!entity.HasComponent<CartesianGridComponent>() && ImGui::MenuItem("Repere cartesien")) {
                entity.AddComponent<CartesianGridComponent>();
                ImGui::CloseCurrentPopup();
            }
            if (!entity.HasComponent<FunctionCurveComponent>() && ImGui::MenuItem("Fonction y = f(x)")) {
                entity.AddComponent<FunctionCurveComponent>();
                ImGui::CloseCurrentPopup();
            }
            if (!entity.HasComponent<TransformAnimationComponent>() && ImGui::MenuItem("Animation de transformation")) {
                auto& animation = entity.AddComponent<TransformAnimationComponent>();
                animation.TriggerSlides = {m_Context->GetCurrentSlide()};
                animation.Start = animation.End = entity.GetComponent<TransformComponent>();
                if (entity.HasComponent<SpriteRendererComponent>()) animation.ColorTarget = AnimationColorTarget::Sprite;
                else if (entity.HasComponent<TextComponent>()) animation.ColorTarget = AnimationColorTarget::Text;
                else if (entity.HasComponent<CartesianGridComponent>()) animation.ColorTarget = AnimationColorTarget::Grid;
                else if (entity.HasComponent<FunctionCurveComponent>()) animation.ColorTarget = AnimationColorTarget::Function;
                animation.StartColor = animation.EndColor = m_Context->GetRenderColor(entity, animation.ColorTarget);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::PopItemWidth();
        DrawComponent<TransformComponent>("Transform", entity, [](auto& component){
            DrawVec3Control("Translation", component.Translation);
            glm::vec3 rotation = glm::degrees(component.Rotation);
            DrawVec3Control("Rotation", rotation);
            component.Rotation = glm::radians(rotation);
            DrawVec3Control("Scale", component.Scale, 1.0f);
        });
        DrawComponent<CameraComponent>("Camera", entity, [](auto& component){
            auto& camera = component.Camera;
            ImGui::Checkbox("Primary", &component.Primary);
            const char* projectionTypeStrings[] = { "Perspective", "Orthographic"};
            const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
            
            if(ImGui::BeginCombo("Projection", currentProjectionTypeString)){
                for(int i = 0; i< 2; i++){
                    bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                    if(ImGui::Selectable(projectionTypeStrings[i], isSelected)){
                        currentProjectionTypeString = projectionTypeStrings[i];
                        camera.SetProjectionType((SceneCamera::ProjectionType)i);
                    }
                    if(isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if(camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective){
                float perspectiveFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
                if(ImGui::DragFloat("FOV", &perspectiveFOV)) 
                    camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveFOV));
                float perspectiveNear = camera.GetPerspectiveNearClip();
                if(ImGui::DragFloat("Near", &perspectiveNear)) 
                    camera.SetPerspectiveNearClip(perspectiveNear);
                float perspectiveFar = camera.GetPerspectiveFarClip();
                if(ImGui::DragFloat("Far", &perspectiveFar))
                    camera.SetPerspectiveFarClip(perspectiveFar);
            }
            if(camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic){
                float orthoSize = camera.GetOrthographicSize();
                if(ImGui::DragFloat("Size", &orthoSize)) 
                    camera.SetOrthographicSize(orthoSize);
                float orthoNear = camera.GetOrthographicNearClip();
                if(ImGui::DragFloat("Near", &orthoNear)) 
                    camera.SetOrthographicNearClip(camera.GetOrthographicNearClip());
                float orthoFar = camera.GetOrthographicFarClip();
                if(ImGui::DragFloat("Far", &orthoFar))
                    camera.SetOrthographicFarClip(camera.GetOrthographicFarClip());
                
                ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
            }
        });
        DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component){
            ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
            if (ImGui::Button("Choisir une texture...")) {
                const auto path = FileDialogs::OpenFile("Images (*.png;*.jpg;*.jpeg;*.bmp;*.tga)\0*.png;*.jpg;*.jpeg;*.bmp;*.tga\0Tous les fichiers\0*.*\0");
                if (!path.empty()) component.SetTexture(path);
            }
            if (!component.TexturePath.empty()) {
                ImGui::TextWrapped("%s", component.TexturePath.c_str());
                if (ImGui::Button("Retirer la texture")) {
                    component.Texture.reset();
                    component.TexturePath.clear();
                    component.TextureError.clear();
                }
            }
            if (!component.TextureError.empty()) ImGui::TextWrapped("%s", component.TextureError.c_str());
            if (component.Texture) {
                const float available = std::max(1.0f, std::min(160.0f, ImGui::GetContentRegionAvail().x));
                const float factor = std::min(available / component.Texture->GetWidth(), 160.0f / component.Texture->GetHeight());
                const float width = component.Texture->GetWidth() * factor;
                const float height = component.Texture->GetHeight() * factor;
                ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(component.Texture->GetRendererID())),
                    ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::DragFloat("Repetition", &component.TilingFactor, 0.1f, 0.1f, 100.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
            }
        }, m_Context->GetCurrentSlide());

        DrawComponent<TextComponent>("Texte", entity, [](auto& text) {
            ImGui::Checkbox("Visible", &text.Visible);
            ImGui::ColorEdit4("Couleur", glm::value_ptr(text.Color));
            ImGui::InputTextMultiline("Contenu",&text.Source,ImVec2(-1.0f, 160.0f));
            ImGui::DragFloat("Taille", &text.FontSize,0.5f, 1.0f, 256.0f, "%.1f",ImGuiSliderFlags_AlwaysClamp);
            ImGui::DragFloat("Interligne", &text.LineSpacing,0.5f, 0.0f, 100.0f, "%.1f",ImGuiSliderFlags_AlwaysClamp);
            ImGui::DragInt("Largeur du texte", &text.MaxWidth,1.0f, 1, 4000, "%d",ImGuiSliderFlags_AlwaysClamp);
        }, m_Context->GetCurrentSlide());
        DrawComponent<CartesianGridComponent>("Repere cartesien", entity, [](auto& grid) {
            ImGui::Checkbox("Visible", &grid.Visible);
            ImGui::DragInt2("Mailles par demi-axe", glm::value_ptr(grid.HalfCells), 1.0f, 1, 1000, "%d", ImGuiSliderFlags_AlwaysClamp);
            ImGui::DragFloat2("Pas X / Y", glm::value_ptr(grid.CellSize), 0.05f, 0.001f, 1000.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            DrawCurveStyle(grid.Style);
        }, m_Context->GetCurrentSlide());
        DrawComponent<FunctionCurveComponent>("Fonction y = f(x)", entity, [](auto& curve) {
            ImGui::Checkbox("Visible", &curve.Visible);
            ImGui::InputText("f(x)", &curve.Expression);
            ImGui::TextWrapped("Exemples : sin(x), x^2, exp(-x^2), 1/x. Angles en radians.");
            ImGui::InputDouble("X min", &curve.MinX, 0.1, 1.0, "%.4f");
            ImGui::InputDouble("X max", &curve.MaxX, 0.1, 1.0, "%.4f");
            ImGui::DragInt("Segments", &curve.Segments, 1.0f, 2, 16384, "%d", ImGuiSliderFlags_AlwaysClamp);
            DrawCurveStyle(curve.Style);
            curve.GetPath();
            if (!curve.GetError().empty())
                ImGui::TextWrapped("%s", curve.GetError().c_str());
        }, m_Context->GetCurrentSlide());
        DrawComponent<TransformAnimationComponent>("Animation de transformation", entity, [&](auto& animation) {
            ImGui::Checkbox("Active", &animation.Enabled);
            ImGui::TextUnformatted("Declencher a l'entree sur les slides");
            DrawSlideList(animation.TriggerSlides, false, m_Context->GetCurrentSlide());
            ImGui::DragFloat("Duree (s)", &animation.Duration, 0.05f, 0.0f, 3600.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            int easing = static_cast<int>(animation.Easing);
            if (ImGui::Combo("Interpolation", &easing, "Lineaire\0Smoothstep\0Ease in/out cubic\0"))
                animation.Easing = static_cast<EasingType>(easing);
            ImGui::Checkbox("Animer la position", &animation.AnimatePosition);
            ImGui::Checkbox("Animer la rotation", &animation.AnimateRotation);
            ImGui::Checkbox("Animer l'echelle", &animation.AnimateScale);
            ImGui::Checkbox("Animer la couleur", &animation.AnimateColor);
            if (animation.AnimateColor) {
                DrawColorTarget("Couleur a animer", animation.ColorTarget);
                glm::vec4 ignored;
                if (!m_Context->TryGetRenderColor(entity, animation.ColorTarget, ignored))
                    ImGui::TextWrapped("Ajouter le composant correspondant a cette entite pour afficher sa couleur.");
            }
            auto drawPose = [&](const char* label, TransformComponent& pose, glm::vec4& color,
                                AnimationValueSource& transformSource, AnimationValueSource& colorSource) {
                ImGui::PushID(label);
                if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen)) {
                    if (animation.AnimatePosition || animation.AnimateRotation || animation.AnimateScale) {
                        ImGui::TextUnformatted("Transformation");
                        DrawAnimationSource("TransformSource", transformSource, *m_Context, false);
                        if (ImGui::Button("Copier la transformation d'edition")) pose = entity.GetComponent<TransformComponent>();
                        if (animation.AnimatePosition) DrawVec3Control("Position", pose.Translation);
                        if (animation.AnimateRotation) {
                            auto degrees = glm::degrees(pose.Rotation);
                            DrawVec3Control("Rotation", degrees);
                            pose.Rotation = glm::radians(degrees);
                        }
                        if (animation.AnimateScale) DrawVec3Control("Echelle", pose.Scale, 1.0f);
                    }
                    if (animation.AnimateColor) {
                        ImGui::TextUnformatted("Couleur");
                        DrawAnimationSource("ColorSource", colorSource, *m_Context, true);
                        ImGui::ColorEdit4("Valeur couleur", glm::value_ptr(color));
                        if (ImGui::Button("Copier la couleur affichee")) color = m_Context->GetRenderColor(entity, animation.ColorTarget);
                    }
                    ImGui::TreePop();
                }
                ImGui::PopID();
            };
            drawPose("Depart", animation.Start, animation.StartColor, animation.StartTransformSource, animation.StartColorSource);
            drawPose("Arrivee", animation.End, animation.EndColor, animation.EndTransformSource, animation.EndColorSource);
            ImGui::BeginDisabled(!animation.Enabled || !m_Context->GetAnimateSlides() ||
                animation.TriggerSlides.empty() || !m_Context->IsVisibleOnCurrentSlide(animation.TriggerSlides));
            if (ImGui::Button("Rejouer cette animation")) {
                animation.LastSlide = 0;
            }
            ImGui::EndDisabled();
            ImGui::TextWrapped("Depart capture au lancement, cible suivie pendant l'animation. La derniere valeur reste affichee apres la fin, meme sur une autre slide.");
            if (!animation.ReferenceError.empty()) ImGui::TextWrapped("%s", animation.ReferenceError.c_str());
        });
   }
}
