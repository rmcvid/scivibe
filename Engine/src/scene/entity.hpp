#include "entt/entt.hpp"
#include "scene/scene.hpp"
#include "pch/pch.hpp"
#include "core/log.hpp"

namespace scivibe{
    class SCIVIBE_API Entity{
        public:
            Entity() = default;
            Entity(entt::entity handle, Scene* scene);
            Entity(const Entity& other) = default;

            template<typename T, typename ... Args>
            T& AddComponent(Args&&... args){
                SCIVIBE_ASSERT(!HasComponent<T>(), "Entity already has component");
                return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
            }

            template<typename T>
            T& GetComponent(){
                SCIVIBE_ASSERT(HasComponent<T>(), "Entity does not have component");
                return m_Scene->m_Registry.get<T>(m_EntityHandle);
            }

            template<typename T>
            bool HasComponent() const {
                return m_Scene && m_Scene->m_Registry.valid(m_EntityHandle)
                    && m_Scene->m_Registry.all_of<T>(m_EntityHandle);
            }

            template<typename T>
            void RemoveComponent(){
                SCIVIBE_ASSERT(HasComponent<T>(), "Entity does not have component");
                m_Scene->m_Registry.remove<T>(m_EntityHandle);
            }

            operator bool() const { return m_EntityHandle != entt::null; }
        private:
            entt::entity m_EntityHandle{entt::null};
            Scene* m_Scene = nullptr ; // ici c'est mieux de mettre un ref non ?
    };
}