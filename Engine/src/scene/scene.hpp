#pragma once
#include "entt/entt.hpp"
#include "pch/pch.hpp"
#include "editor/editorCamera.hpp"
// #include "scene/component.hpp" retirer la circulaire pour ne pas devoir déclarer les composants
// comment faire pour ne pas donner tous les tempaltes, ils devraient deja etre définie non ?


namespace scivibe{
    class Entity;
    struct TransformComponent;
    enum class AnimationColorTarget;
    class SceneHierarchyPanel;
    class SCIVIBE_API Scene{
    public:
        Scene();
        ~Scene();
        Entity CreateEntity(const std::string& name = std::string());
        void DestroyEntity(Entity entity);
        entt::registry* Reg() { return &m_Registry;}
        void OnUpdateRunTime(Timestep ts);
        void OnUpdateEditor(Timestep ts, EditorCamera& camera);
        void OnViewPortResize(uint32_t width, uint32_t height );
        Entity GetPrimaryCameraEntity();
        int GetCurrentSlide() const { return m_CurrentSlide; }
        void SetCurrentSlide(int slide);
        bool IsVisibleOnCurrentSlide(const std::vector<int>& slides) const;
        void RestartSlideAnimations();
        bool GetAnimateSlides() const { return m_AnimateSlides; }
        void SetAnimateSlides(bool enabled);
        TransformComponent GetRenderTransform(entt::entity entity) const;
        bool HasAnimatedPose(entt::entity entity) const;
        glm::vec4 GetRenderColor(entt::entity entity, AnimationColorTarget target) const;
        bool TryGetRenderColor(entt::entity entity, AnimationColorTarget target, glm::vec4& color) const;
        void ResetAnimationPreview();
    private:
        int m_CurrentSlide = 1;
        bool m_AnimateSlides = true;
        void UpdateSlideAnimations(Timestep ts);
        void RenderContents();
        void OnCameraConstructed(entt::registry& registry, entt::entity entity);
        uint32_t m_ViewportWidth =0 , m_ViewportHeight =0 ; 
        entt::registry m_Registry;
        friend class Entity;
        friend class SceneHierarchyPanel;
        friend class SceneSerializer;
    };
    
    
}
