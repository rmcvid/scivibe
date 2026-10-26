#include "scene/entity.hpp"
namespace scivibe{
    Entity::Entity(entt::entity handle, Scene* scene)
        : m_EntityHandle(handle), m_Scene(scene) {
    }
}