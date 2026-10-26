#pragma once
#include "entt/entt.hpp"
#include "pch/pch.hpp"



namespace scivibe{
    class Entity;

    class SCIVIBE_API Scene{
    public:
        Scene();
        ~Scene();
        Entity CreateEntity(const std::string& name = std::string());
        entt::registry* Reg() { return &m_Registry;}
        void OnUpdate(Timestep ts);
    private:
        entt::registry m_Registry;
        friend class Entity;
    };
    
}
