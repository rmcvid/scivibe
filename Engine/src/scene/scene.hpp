#pragma once
#include "entt/entt.hpp"
#include "pch/pch.hpp"
// #include "scene/component.hpp" retirer la circulaire pour ne pas devoir déclarer les composants
// comment faire pour ne pas donner tous les tempaltes, ils devraient deja etre définie non ?


namespace scivibe{
    class Entity;
    class SceneHierarchyPanel;

    struct CameraComponent;
    struct TransformComponent;
    struct SpriteRendererComponent;
    struct TagComponent;
    struct NativeScriptComponent;
    class SCIVIBE_API Scene{
    public:
        Scene();
        ~Scene();
        Entity CreateEntity(const std::string& name = std::string());
        void DestroyEntity(Entity entity);
        entt::registry* Reg() { return &m_Registry;}
        void OnUpdate(Timestep ts);
        void OnViewPortResize(uint32_t width, uint32_t height );
    private:
        template<typename T>
        void OnComponentAdded(Entity entity, T& component);

        uint32_t m_ViewportWidth =0 , m_ViewportHeight =0 ; 
        entt::registry m_Registry;
        friend class Entity;
        friend class SceneHierarchyPanel;
        friend class SceneSerializer;
    };
    template<>
    SCIVIBE_API void Scene::OnComponentAdded<CameraComponent>(
        Entity entity, CameraComponent& component);

    template<>
    SCIVIBE_API void Scene::OnComponentAdded<TransformComponent>(
        Entity entity, TransformComponent& component);

    template<>
    SCIVIBE_API void Scene::OnComponentAdded<SpriteRendererComponent>(
        Entity entity, SpriteRendererComponent& component);

    template<>
    SCIVIBE_API void Scene::OnComponentAdded<TagComponent>(
        Entity entity, TagComponent& component);

    template<>
    SCIVIBE_API void Scene::OnComponentAdded<NativeScriptComponent>(
        Entity entity, NativeScriptComponent& component);
    
}
