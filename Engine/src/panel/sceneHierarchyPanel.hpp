#pragma once
#include "scene/scene.hpp"
#include "core/core.hpp"
#include "scene/entity.hpp"

namespace scivibe{
    class SCIVIBE_API SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& scene);
        void SetContext(const Ref<Scene>& scene);
        void OnImGuiRender();
    private:
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity); 

        Ref<Scene> m_Context;
        Entity m_SelectionContext;
        friend class Scene;
    };
}