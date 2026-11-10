#include "pch/pch.hpp"
#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "scene/component.hpp"
#include "renderer/renderer2D.hpp"
namespace scivibe{
    //Scene::Scene() = default;
    Scene::Scene(){
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
    void Scene::DestroyEntity(Entity entity){
        m_Registry.destroy(entity);
    }

    void Scene::OnUpdate(Timestep ts){
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

        {
            // Views can share TransformComponent without taking ownership of its storage.
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view){
                auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
                if(camera.Primary){
                    mainCamera = &camera.Camera;
                    cameraTransform = transform.GetTransform();
                    break;
                }
            }
        }
        if(mainCamera){
            Renderer2D::BeginScene(*mainCamera, cameraTransform);
            auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
            for(auto entity : view){
                auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);
                Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
            }
            Renderer2D::EndScene();
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
    template<typename T>
    void Scene::OnComponentAdded(Entity entity, T& component){
        // Implementation for handling component addition
    }
    template<>
    void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component){
        component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }
    template<>
    void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component){
        // Implementation for handling TransformComponent addition
    }
    template<>
    void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component){
        // Implementation for handling SpriteRendererComponent addition
    }
    template<>
    void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component){
        // Implementation for handling TagComponent addition
    }
    template<>
    void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component){
        // Implementation for handling NativeScriptComponent addition
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