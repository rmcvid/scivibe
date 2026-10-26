#include "pch/pch.hpp"
#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "scene/component.hpp"
#include "renderer/renderer2D.hpp"
namespace scivibe{
    static void DoMath(const glm::mat4 transform){

    }
    Scene::Scene(){
        struct MeshComponent{
            float f;
            MeshComponent() = default;
        };
        
        TransformComponent transform;
        DoMath(transform);

        entt::entity entity = m_Registry.create();
        m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));
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

    }
    Scene::~Scene(){
    }
    Entity Scene::CreateEntity(const std::string& name){
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        return entity;
    }
    
    void Scene::OnUpdate(Timestep ts){
        auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
        for(auto entity : view){
            auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);
            Renderer2D::DrawQuad(transform, sprite.Color);
        }
    }
}
