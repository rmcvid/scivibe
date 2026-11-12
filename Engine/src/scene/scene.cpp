#include "pch/pch.hpp"
#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "scene/component.hpp"
#include "renderer/renderer2D.hpp"
#include "renderer/renderCommand.hpp"
#include "pseudoLatex/pseudoLatex.hpp"
#include "animator/interpolator.hpp"
#include <cmath>
namespace scivibe{
    //Scene::Scene() = default;
    Scene::Scene() {
        m_Registry.on_construct<CameraComponent>()
            .connect<&Scene::OnCameraConstructed>(*this);
    }
    Scene::~Scene(){
    }
    void Scene::SetCurrentSlide(int slide) {
        const int next = std::clamp(slide, 1, 1000000);
        if (m_CurrentSlide == next) return;
        m_CurrentSlide = next;
        // Re-arm entry detection without discarding the result of earlier slides.
        for (auto entity : m_Registry.view<TransformAnimationComponent>())
            m_Registry.get<TransformAnimationComponent>(entity).LastSlide = 0;
    }
    bool Scene::IsVisibleOnCurrentSlide(const std::vector<int>& slides) const {
        return slides.empty() || std::find(slides.begin(), slides.end(), m_CurrentSlide) != slides.end();
    }
    void Scene::RestartSlideAnimations() {
        auto animations = m_Registry.view<TransformAnimationComponent>();
        for (auto entity : animations) {
            auto& animation = animations.get<TransformAnimationComponent>(entity);
            if (!animation.TriggerSlides.empty() && IsVisibleOnCurrentSlide(animation.TriggerSlides))
                animation.LastSlide = 0;
        }
    }
    void Scene::SetAnimateSlides(bool enabled) {
        if (m_AnimateSlides == enabled) return;
        m_AnimateSlides = enabled;
        RestartSlideAnimations();
    }
    bool Scene::HasAnimatedPose(entt::entity entity) const {
        if (!m_Registry.valid(entity)) return false;
        const auto* animation = m_Registry.try_get<TransformAnimationComponent>(entity);
        return m_AnimateSlides && animation && animation->Enabled && animation->HasPose &&
            (animation->AnimatePosition || animation->AnimateRotation || animation->AnimateScale);
    }
    void Scene::ResetAnimationPreview() {
        for (auto entity : m_Registry.view<TransformAnimationComponent>()) {
            auto& animation = m_Registry.get<TransformAnimationComponent>(entity);
            animation.HasPose = false;
            animation.Running = false;
            animation.LastSlide = m_CurrentSlide;
            animation.ReferenceError.clear();
            animation.StartReferenceError.clear();
        }
    }
    bool Scene::TryGetRenderColor(entt::entity entity, AnimationColorTarget target, glm::vec4& color) const {
        if (!m_Registry.valid(entity)) return false;
        switch (target) {
            case AnimationColorTarget::Sprite: {
                const auto* component = m_Registry.try_get<SpriteRendererComponent>(entity);
                if (!component) return false;
                color = component->Color; break;
            }
            case AnimationColorTarget::Text: {
                const auto* component = m_Registry.try_get<TextComponent>(entity);
                if (!component) return false;
                color = component->Color; break;
            }
            case AnimationColorTarget::Grid: {
                const auto* component = m_Registry.try_get<CartesianGridComponent>(entity);
                if (!component) return false;
                color = component->Style.Color; break;
            }
            case AnimationColorTarget::Function: {
                const auto* component = m_Registry.try_get<FunctionCurveComponent>(entity);
                if (!component) return false;
                color = component->Style.Color; break;
            }
            default: return false;
        }
        const auto* animation = m_Registry.try_get<TransformAnimationComponent>(entity);
        if (m_AnimateSlides && animation && animation->Enabled && animation->HasPose &&
            animation->AnimateColor && animation->ColorTarget == target) color = animation->Color;
        return true;
    }
    glm::vec4 Scene::GetRenderColor(entt::entity entity, AnimationColorTarget target) const {
        glm::vec4 color(1.0f);
        TryGetRenderColor(entity, target, color);
        return color;
    }
    TransformComponent Scene::GetRenderTransform(entt::entity entity) const {
        auto transform = m_Registry.get<TransformComponent>(entity);
        if (HasAnimatedPose(entity)) {
            const auto& animation = m_Registry.get<TransformAnimationComponent>(entity);
            if (animation.AnimatePosition) transform.Translation = animation.Pose.Translation;
            if (animation.AnimateRotation) transform.Rotation = animation.Pose.Rotation;
            if (animation.AnimateScale) transform.Scale = animation.Pose.Scale;
        }
        return transform;
    }
    void Scene::UpdateSlideAnimations(Timestep ts) {
        if (!m_AnimateSlides) return;
        // Read a consistent snapshot before writing any animation outputs. This
        // avoids iteration-order dependence and recursion for cyclic references.
        // Script/physics writes to TransformComponent are already visible here.
        struct Values {
            TransformComponent Transform;
            std::array<glm::vec4, 4> Colors;
            std::array<bool, 4> HasColor{};
        };
        std::unordered_map<entt::entity, Values> values;
        for (auto entity : m_Registry.view<TransformComponent>()) {
            auto& entry = values[entity];
            entry.Transform = GetRenderTransform(entity);
            for (int i = 0; i < 4; ++i)
                entry.HasColor[i] = TryGetRenderColor(entity, static_cast<AnimationColorTarget>(i), entry.Colors[i]);
        }
        const double seconds = ts.GetSeconds();
        const double step = std::isfinite(seconds) ? std::max(0.0, seconds) : 0.0;
        auto animations = m_Registry.view<TransformComponent, TransformAnimationComponent>();
        for (auto entity : animations) {
            auto& animation = animations.get<TransformAnimationComponent>(entity);
            if (!animation.Enabled) {
                animation.LastSlide = 0;
                animation.HasPose = false;
                animation.Running = false;
                continue;
            }
            auto readTransform = [&](const AnimationValueSource& source, TransformComponent& result) {
                if (!source.UseEntity) return;
                const auto it = values.find(source.Entity);
                if (it == values.end()) {
                    animation.ReferenceError = "Entite source absente : derniere valeur connue conservee (ou valeur manuelle au lancement).";
                    return;
                }
                const auto& pose = it->second.Transform;
                for (int i = 0; i < 3; ++i) {
                    if (!std::isfinite(pose.Translation[i]) || !std::isfinite(pose.Rotation[i]) || !std::isfinite(pose.Scale[i])) {
                        animation.ReferenceError = "Transformation source non finie : derniere valeur connue conservee.";
                        return;
                    }
                }
                result = pose;
            };
            auto readColor = [&](const AnimationValueSource& source, glm::vec4& result) {
                if (!source.UseEntity) return;
                const auto it = values.find(source.Entity);
                const int target = static_cast<int>(source.ColorTarget);
                if (it == values.end() || target < 0 || target >= 4 || !it->second.HasColor[target]) {
                    animation.ReferenceError = "Couleur source absente : derniere valeur connue conservee (ou valeur manuelle au lancement).";
                    return;
                }
                const auto color = it->second.Colors[target];
                for (int i = 0; i < 4; ++i) if (!std::isfinite(color[i])) {
                    animation.ReferenceError = "Couleur source non finie : derniere valeur connue conservee.";
                    return;
                }
                result = color;
            };
            const bool entering = animation.LastSlide != m_CurrentSlide &&
                !animation.TriggerSlides.empty() && IsVisibleOnCurrentSlide(animation.TriggerSlides);
            animation.LastSlide = m_CurrentSlide;
            if (entering) {
                animation.Elapsed = 0.0;
                animation.Running = true;
                animation.ReferenceError.clear();
                animation.CapturedStart = animation.Start;
                animation.CapturedEnd = animation.End;
                animation.CapturedStartColor = animation.StartColor;
                animation.CapturedEndColor = animation.EndColor;
                readTransform(animation.StartTransformSource, animation.CapturedStart);
                if (animation.AnimateColor) readColor(animation.StartColorSource, animation.CapturedStartColor);
                animation.StartReferenceError = animation.ReferenceError;
            } else if (animation.Running) {
                animation.Elapsed += step;
            }
            if (!animation.Running) continue; // Hold the last result, also on other slides.
            animation.ReferenceError = animation.StartReferenceError;
            if (!animation.EndTransformSource.UseEntity) animation.CapturedEnd = animation.End;
            readTransform(animation.EndTransformSource, animation.CapturedEnd);
            if (animation.AnimateColor) {
                if (!animation.EndColorSource.UseEntity) animation.CapturedEndColor = animation.EndColor;
                readColor(animation.EndColorSource, animation.CapturedEndColor);
            }
            const float t = std::isfinite(animation.Duration) && animation.Duration > 0.0f
                ? static_cast<float>(std::min(animation.Elapsed / animation.Duration, 1.0)) : 1.0f;
            const float eased = Easing::Apply(t, animation.Easing);
            animation.Pose.Translation = Interpolator<glm::vec3>::Interpolate(animation.CapturedStart.Translation, animation.CapturedEnd.Translation, eased);
            animation.Pose.Rotation = Interpolator<glm::vec3>::Interpolate(animation.CapturedStart.Rotation, animation.CapturedEnd.Rotation, eased);
            animation.Pose.Scale = Interpolator<glm::vec3>::Interpolate(animation.CapturedStart.Scale, animation.CapturedEnd.Scale, eased);
            animation.Color = glm::clamp(Interpolator<glm::vec4>::Interpolate(animation.CapturedStartColor, animation.CapturedEndColor, eased), glm::vec4(0), glm::vec4(1));
            animation.HasPose = true;
            if (t >= 1.0f) animation.Running = false;
        }
    }
    void Scene::OnCameraConstructed(entt::registry& registry,entt::entity entity){
        auto& component = registry.get<CameraComponent>(entity);
        if (m_ViewportWidth > 0 && m_ViewportHeight > 0) {
            component.Camera.SetViewportSize(
                m_ViewportWidth,
                m_ViewportHeight
            );
        }
    }
    Entity Scene::CreateEntity(const std::string& name){
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        
        return entity;
    }
    void Scene::DestroyEntity(Entity entity){
        m_Registry.destroy(entity);
    }
    Entity Scene::GetPrimaryCameraEntity(){
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view){
            const auto& camera = view.get<CameraComponent>(entity);
            if(camera.Primary){
                return Entity{entity,this};
            }
        }
        return {};
    }
    void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera){
        UpdateSlideAnimations(ts);
        Renderer2D::BeginScene(camera);
        RenderContents();
        Renderer2D::EndScene();
    }
    void Scene::OnUpdateRunTime(Timestep ts){
        const Camera* mainCamera = nullptr;
        glm::mat4 cameraTransform;
        // update scripts
        {
            m_Registry.view<NativeScriptComponent>().each([=,this](auto entity, auto &nsc){
                // ici move to scene::OnScenePlay ?
                if(! nsc.Instance){
                    nsc.Instance = nsc.InstantiateScript();
                    nsc.Instance->m_Entity = Entity{entity , this};
                    nsc.Instance->OnCreate();
                }
                nsc.Instance->OnUpdate(ts);
            });
        }
        UpdateSlideAnimations(ts);
        {
            // Views can share TransformComponent without taking ownership of its storage.
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view){
                auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
                if(camera.Primary){
                    mainCamera = &camera.Camera;
                    cameraTransform = GetRenderTransform(entity).GetTransform();
                    break;
                }
            }
        }
        if(mainCamera){
            Renderer2D::BeginScene(*mainCamera, cameraTransform);
            RenderContents();
            Renderer2D::EndScene();
        }
    }
    void Scene::RenderContents() {
        auto sprites = m_Registry.view<TransformComponent, SpriteRendererComponent>();
        for (auto entity : sprites) {
            auto [transform, sprite] = sprites.get<TransformComponent, SpriteRendererComponent>(entity);
            if (!IsVisibleOnCurrentSlide(sprite.Slides)) continue;
            const auto matrix = GetRenderTransform(entity).GetTransform();
            const auto color = GetRenderColor(entity, AnimationColorTarget::Sprite);
            if (sprite.Texture)
                Renderer2D::DrawQuad(matrix, sprite.Texture, {0, 0}, {1, 1}, sprite.TilingFactor, color);
            else
                Renderer2D::DrawQuad(matrix, color);
        }
        Renderer2D::Flush();

        // A coplanar grid must not hide the function drawn on top of it.
        {
            struct GridDepthScope {
                GridDepthScope() { RenderCommand::SetDepthWrite(false); }
                ~GridDepthScope() { RenderCommand::SetDepthWrite(true); }
            } depthScope;
            auto grids = m_Registry.view<TransformComponent, CartesianGridComponent>();
            for (auto entity : grids) {
                auto [transform, grid] = grids.get<TransformComponent, CartesianGridComponent>(entity);
                if (grid.Visible && IsVisibleOnCurrentSlide(grid.Slides)) {
                    auto style = grid.Style;
                    style.Color = GetRenderColor(entity, AnimationColorTarget::Grid);
                    RendererCurve2D::DrawCarthesian({0, 0}, glm::dvec2(grid.HalfCells),
                        glm::dvec2(grid.CellSize), style, {0, 0}, {0, constants::Pi / 2}, GetRenderTransform(entity).GetTransform());
                }
            }
            Renderer2D::Flush();
        }

        auto curves = m_Registry.view<TransformComponent, FunctionCurveComponent>();
        for (auto entity : curves) {
            auto [transform, curve] = curves.get<TransformComponent, FunctionCurveComponent>(entity);
            if (curve.Visible && IsVisibleOnCurrentSlide(curve.Slides)) {
                auto style = curve.Style;
                style.Color = GetRenderColor(entity, AnimationColorTarget::Function);
                RendererCurve2D::DrawPath(curve.GetPath(), style, GetRenderTransform(entity).GetTransform());
            }
        }
        auto texts = m_Registry.view<TransformComponent, TextComponent>();
        for (auto entity : texts) {
            auto [transform, text] = texts.get<TransformComponent, TextComponent>(entity);
            if (text.Visible && !text.Source.empty() && IsVisibleOnCurrentSlide(text.Slides))
                PseudoLatex::Write(text.Source, GetRenderTransform(entity).GetTransform(), text.FontSize, text.MaxWidth, text.LineSpacing,
                    GetRenderColor(entity, AnimationColorTarget::Text));
        }
    }
    void Scene::OnViewPortResize(uint32_t width, uint32_t height){
        m_ViewportWidth = width;
        m_ViewportHeight = height;
        auto view = m_Registry.view<CameraComponent>();
        for(auto entity : view) {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if(!cameraComponent.FixedAspectRatio){
                cameraComponent.Camera.SetViewportSize(width, height);
            }
        }

    }
}

/*
Dans le constructeur pour la 3d :
struct MeshComponent{
            float f;
            MeshComponent() = default;
        };
        
        TransformComponent transform;
        entt::entity entity = m_Registry.create();
        m_Registry.emplace<TransformComponent>(entity);
        if(m_Registry.all_of<TransformComponent>(entity))
            TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
        auto view = m_Registry.view<TransformComponent>();
        for(auto entity : view){
            TransformComponent& transform = view.get<TransformComponent>(entity);
        }
        auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
        for(auto entity : group){
            auto[transform, mesh] = group.get<TransformComponent,MeshComponent>(entity);
        }
*/
