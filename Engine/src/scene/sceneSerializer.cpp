#include "scene/sceneSerializer.hpp"
#include "pch/pch.hpp"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include "entity.hpp"
#include "scene/component.hpp"

namespace YAML{
    template<> 
    struct convert<glm::vec3>{
        static Node encode(const glm::vec3& rhs){
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }
        static bool decode(const Node& node, glm::vec3& rhs){
            if(!node.IsSequence() || node.size() != 3)
                return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };
    template<>
    struct convert<glm::vec4>{
        static Node encode(const glm::vec4& rhs){
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }
        static bool decode(const Node& node, glm::vec4& rhs){
            if(!node.IsSequence() || node.size() != 4)
                return false;
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };
}
namespace scivibe{
    SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : m_Scene(scene) {}

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    static void SerializeEntity(YAML::Emitter& out, Entity entity) {
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << static_cast<uint32_t>(entity);
        if(entity.HasComponent<TagComponent>()) {
            auto& tag = entity.GetComponent<TagComponent>();
            out << YAML::Key << "TagComponent" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "Tag" << YAML::Value << tag.Tag;
            out << YAML::EndMap;
        }

        if(entity.HasComponent<TransformComponent>()) {
            auto& transform = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "TransformComponent" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "Translation" << YAML::Value << transform.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << transform.Scale;
            out << YAML::EndMap;
        }
        if(entity.HasComponent<CameraComponent>()) {
            out << YAML::Key << "CameraComponent" << YAML::Value << YAML::BeginMap; // CameraComponent
            auto& cameraComponent = entity.GetComponent<CameraComponent>();
            auto& camera = cameraComponent.Camera;
            out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap; // Camera
            out << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int>(camera.GetProjectionType());
            out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
            out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
            out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
            out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
            out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
            out << YAML::EndMap; // Camera
            out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;
            out << YAML::EndMap; // CameraComponent
        }
        if(entity.HasComponent<SpriteRendererComponent>()) {
            auto& sprite = entity.GetComponent<SpriteRendererComponent>();
            out << YAML::Key << "SpriteRendererComponent" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "Color" << YAML::Value << sprite.Color;
            out << YAML::EndMap;
        }
        
        out << YAML::EndMap; // Entity
    }

    void SceneSerializer::Serialize(const std::string& filepath) {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        for (auto entityID : m_Scene->m_Registry.storage<entt::entity>()) {
            Entity entity{entityID, m_Scene.get()};
            if (!entity)
                continue;
            SerializeEntity(out, entity);
        }
        out << YAML::EndSeq;
        out << YAML::EndMap;
        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    void SceneSerializer::SerializeRuntime(const std::string& filepath) {
        // Implementation for serializing runtime scene
    }

    bool SceneSerializer::Deserialize(const std::string& filepath) {
        std::ifstream stream(filepath);
        std::stringstream strStream;
        strStream << stream.rdbuf();
        YAML::Node data = YAML::Load(strStream.str());
        if (!data["Scene"]){
            SCIVIBE_CORE_ERROR("SceneSerializer: Scene node not found in YAML file.");
            return false;
        }
        std::string sceneName = data["Scene"].as<std::string>();
        SCIVIBE_CORE_INFO("Deserializing scene: {}", sceneName);
        auto entities = data["Entities"];
        if (entities) {
            for (const auto& entityNode : entities) {
                std::string name = "Entity";
                const auto tagNode = entityNode["TagComponent"];
                if (tagNode) {name = tagNode["Tag"].as<std::string>();}
                Entity entity = m_Scene->CreateEntity(name);
                const auto transformNode = entityNode["TransformComponent"];
                if (transformNode) {auto& transform = entity.GetComponent<TransformComponent>();
                    transform.Translation = transformNode["Translation"].as<glm::vec3>();
                    transform.Rotation = transformNode["Rotation"].as<glm::vec3>();
                    transform.Scale = transformNode["Scale"].as<glm::vec3>();
                }
                if (entityNode["CameraComponent"]) {
                    auto& cameraComponentNode = entityNode["CameraComponent"];
                    auto& cameraNode = cameraComponentNode["Camera"];
                    CameraComponent cameraComp;
                    cameraComp.Camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(cameraNode["ProjectionType"].as<int>()));
                    cameraComp.Camera.SetPerspectiveVerticalFOV(cameraNode["PerspectiveFOV"].as<float>());
                    cameraComp.Camera.SetPerspectiveNearClip(cameraNode["PerspectiveNear"].as<float>());
                    cameraComp.Camera.SetPerspectiveFarClip(cameraNode["PerspectiveFar"].as<float>());
                    cameraComp.Camera.SetOrthographicSize(cameraNode["OrthographicSize"].as<float>());
                    cameraComp.Camera.SetOrthographicNearClip(cameraNode["OrthographicNear"].as<float>());
                    cameraComp.Camera.SetOrthographicFarClip(cameraNode["OrthographicFar"].as<float>());
                    cameraComp.Primary = cameraComponentNode["Primary"].as<bool>();
                    cameraComp.FixedAspectRatio = cameraComponentNode["FixedAspectRatio"].as<bool>();
                    entity.AddComponent<CameraComponent>(cameraComp);
                }
                if (entityNode["SpriteRendererComponent"]) {
                    auto& spriteRendererNode = entityNode["SpriteRendererComponent"];
                    glm::vec4 color = spriteRendererNode["Color"].as<glm::vec4>();
                    entity.AddComponent<SpriteRendererComponent>(color);
                }
            }
        }
        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string& filepath) {
        SCIVIBE_CORE_ASSERT(false, "Runtime deserialization is not supported yet.");
        return true;
    }
}