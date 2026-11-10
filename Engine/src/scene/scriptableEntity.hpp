#pragma once
#include "entt/entt.hpp"
#include "scene/scene.hpp"
#include "pch/pch.hpp"
#include "core/log.hpp"
#include "scene/entity.hpp"

namespace scivibe{
    class SCIVIBE_API ScriptableEntity{
    public:
        template<typename T>
        T& GetComponent(){
            return m_Entity.GetComponent<T>();
        }
        virtual ~ScriptableEntity(){}
    protected:
        virtual void OnCreate(){}
        virtual void OnUpdate(Timestep ts){}
        virtual void OnDestroy(){}
    private:
        Entity m_Entity;
        friend class Scene;
    };
}