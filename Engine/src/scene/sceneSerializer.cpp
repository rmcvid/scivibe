#include "scene/sceneSerializer.hpp"
#include "pch/pch.hpp"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include "entity.hpp"
#include "scene/component.hpp"
#include <cmath>
#include <filesystem>

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

    static void SerializeCurveStyle(YAML::Emitter& out, const CurveStyle& style) {
        out << YAML::Key << "Color" << YAML::Value << style.Color;
        out << YAML::Key << "WidthPixels" << YAML::Value << style.WidthPixels;
        out << YAML::Key << "Join" << YAML::Value << static_cast<int>(style.Join);
        out << YAML::Key << "Cap" << YAML::Value << static_cast<int>(style.Cap);
        out << YAML::Key << "MiterLimit" << YAML::Value << style.MiterLimit;
    }
    static void DeserializeCurveStyle(const YAML::Node& node, CurveStyle& style) {
        if (node["Color"]) style.Color = node["Color"].as<glm::vec4>();
        const float width = node["WidthPixels"].as<float>(style.WidthPixels);
        const float miter = node["MiterLimit"].as<float>(style.MiterLimit);
        if (std::isfinite(width)) style.WidthPixels = std::clamp(width, 0.1f, 100.0f);
        if (std::isfinite(miter)) style.MiterLimit = std::clamp(miter, 1.0f, 20.0f);
        style.Join = static_cast<CurveJoin>(std::clamp(node["Join"].as<int>(static_cast<int>(style.Join)), 0, 2));
        style.Cap = static_cast<CurveCap>(std::clamp(node["Cap"].as<int>(static_cast<int>(style.Cap)), 0, 2));
    }

    static std::vector<int> ReadSlides(const YAML::Node& node, const std::vector<int>& fallback = {}) {
        if (!node || !node.IsSequence()) return fallback;
        auto slides = node.as<std::vector<int>>();
        slides.erase(std::remove_if(slides.begin(), slides.end(), [](int slide) { return slide < 1 || slide > 1000000; }), slides.end());
        std::sort(slides.begin(), slides.end());
        slides.erase(std::unique(slides.begin(), slides.end()), slides.end());
        return slides;
    }
    static void SerializePose(YAML::Emitter& out, const TransformComponent& pose) {
        out << YAML::BeginMap;
        out << YAML::Key << "Translation" << YAML::Value << pose.Translation;
        out << YAML::Key << "Rotation" << YAML::Value << pose.Rotation;
        out << YAML::Key << "Scale" << YAML::Value << pose.Scale;
        out << YAML::EndMap;
    }
    static void DeserializePose(const YAML::Node& node, TransformComponent& pose) {
        if (!node) return;
        pose.Translation = node["Translation"].as<glm::vec3>(pose.Translation);
        pose.Rotation = node["Rotation"].as<glm::vec3>(pose.Rotation);
        pose.Scale = node["Scale"].as<glm::vec3>(pose.Scale);
    }
    static void SerializeAnimationSource(YAML::Emitter& out, const AnimationValueSource& source) {
        out << YAML::BeginMap;
        out << YAML::Key << "UseEntity" << YAML::Value << source.UseEntity;
        out << YAML::Key << "Entity" << YAML::Value << static_cast<uint32_t>(source.Entity);
        out << YAML::Key << "ColorTarget" << YAML::Value << static_cast<int>(source.ColorTarget);
        out << YAML::EndMap;
    }
    static void DeserializeAnimationSource(const YAML::Node& node, AnimationValueSource& source,
        const std::unordered_map<uint32_t, entt::entity>& entities) {
        if (!node) return;
        source.UseEntity = node["UseEntity"].as<bool>(false);
        source.ColorTarget = static_cast<AnimationColorTarget>(std::clamp(node["ColorTarget"].as<int>(0), 0, 3));
        source.Entity = entt::null;
        if (node["Entity"]) {
            const auto found = entities.find(node["Entity"].as<uint32_t>());
            if (found != entities.end()) source.Entity = found->second;
        }
    }
    static void SerializeEntity(YAML::Emitter& out, Entity entity, const std::filesystem::path& sceneDirectory) {
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
            out << YAML::Key << "Slides" << YAML::Value << YAML::Flow << sprite.Slides;
            out << YAML::Key << "TilingFactor" << YAML::Value << sprite.TilingFactor;
            if (!sprite.TexturePath.empty()) {
                std::error_code error;
                auto relative = std::filesystem::relative(sprite.TexturePath, sceneDirectory, error);
                const auto path = error || relative.empty() ? std::filesystem::path(sprite.TexturePath) : relative;
                out << YAML::Key << "TexturePath" << YAML::Value << path.generic_string();
            }
            out << YAML::EndMap;
        }
        if (entity.HasComponent<TextComponent>()) {
            const auto& text = entity.GetComponent<TextComponent>();
            out << YAML::Key << "TextComponent" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "Source" << YAML::Value << text.Source;
            out << YAML::Key << "Color" << YAML::Value << text.Color;
            out << YAML::Key << "Slides" << YAML::Value << YAML::Flow << text.Slides;
            out << YAML::Key << "Visible" << YAML::Value << text.Visible;
            out << YAML::Key << "FontSize" << YAML::Value << text.FontSize;
            out << YAML::Key << "LineSpacing" << YAML::Value << text.LineSpacing;
            out << YAML::Key << "MaxWidth" << YAML::Value << text.MaxWidth;
            out << YAML::EndMap;
        }
        
        if (entity.HasComponent<CartesianGridComponent>()) {
            const auto& grid = entity.GetComponent<CartesianGridComponent>();
            out << YAML::Key << "CartesianGridComponent" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "Visible" << YAML::Value << grid.Visible;
            out << YAML::Key << "Slides" << YAML::Value << YAML::Flow << grid.Slides;
            out << YAML::Key << "HalfCells" << YAML::Value << YAML::Flow << YAML::BeginSeq << grid.HalfCells.x << grid.HalfCells.y << YAML::EndSeq;
            out << YAML::Key << "CellSize" << YAML::Value << YAML::Flow << YAML::BeginSeq << grid.CellSize.x << grid.CellSize.y << YAML::EndSeq;
            SerializeCurveStyle(out, grid.Style);
            out << YAML::EndMap;
        }
        if (entity.HasComponent<FunctionCurveComponent>()) {
            const auto& curve = entity.GetComponent<FunctionCurveComponent>();
            out << YAML::Key << "FunctionCurveComponent" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "Visible" << YAML::Value << curve.Visible;
            out << YAML::Key << "Expression" << YAML::Value << curve.Expression;
            out << YAML::Key << "Slides" << YAML::Value << YAML::Flow << curve.Slides;
            out << YAML::Key << "MinX" << YAML::Value << curve.MinX;
            out << YAML::Key << "MaxX" << YAML::Value << curve.MaxX;
            out << YAML::Key << "Segments" << YAML::Value << curve.Segments;
            SerializeCurveStyle(out, curve.Style);
            out << YAML::EndMap;
        }
        if (entity.HasComponent<TransformAnimationComponent>()) {
            const auto& animation = entity.GetComponent<TransformAnimationComponent>();
            out << YAML::Key << "TransformAnimationComponent" << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "Enabled" << YAML::Value << animation.Enabled;
            out << YAML::Key << "TriggerSlides" << YAML::Value << YAML::Flow << animation.TriggerSlides;
            out << YAML::Key << "Duration" << YAML::Value << animation.Duration;
            out << YAML::Key << "Easing" << YAML::Value << static_cast<int>(animation.Easing);
            out << YAML::Key << "AnimatePosition" << YAML::Value << animation.AnimatePosition;
            out << YAML::Key << "AnimateRotation" << YAML::Value << animation.AnimateRotation;
            out << YAML::Key << "AnimateScale" << YAML::Value << animation.AnimateScale;
            out << YAML::Key << "AnimateColor" << YAML::Value << animation.AnimateColor;
            out << YAML::Key << "ColorTarget" << YAML::Value << static_cast<int>(animation.ColorTarget);
            out << YAML::Key << "StartColor" << YAML::Value << animation.StartColor;
            out << YAML::Key << "EndColor" << YAML::Value << animation.EndColor;
            out << YAML::Key << "StartTransformSource" << YAML::Value;
            SerializeAnimationSource(out, animation.StartTransformSource);
            out << YAML::Key << "EndTransformSource" << YAML::Value;
            SerializeAnimationSource(out, animation.EndTransformSource);
            out << YAML::Key << "StartColorSource" << YAML::Value;
            SerializeAnimationSource(out, animation.StartColorSource);
            out << YAML::Key << "EndColorSource" << YAML::Value;
            SerializeAnimationSource(out, animation.EndColorSource);
            out << YAML::Key << "Start" << YAML::Value;
            SerializePose(out, animation.Start);
            out << YAML::Key << "End" << YAML::Value;
            SerializePose(out, animation.End);
            out << YAML::EndMap;
        }
        out << YAML::EndMap; // Entity
    }

    void SceneSerializer::Serialize(const std::string& filepath) {
        const auto sceneDirectory = std::filesystem::absolute(filepath).parent_path();
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "CurrentSlide" << YAML::Value << m_Scene->GetCurrentSlide();
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        for (auto entityID : m_Scene->m_Registry.storage<entt::entity>()) {
            Entity entity{entityID, m_Scene.get()};
            if (!entity)
                continue;
            SerializeEntity(out, entity, sceneDirectory);
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
        const auto sceneDirectory = std::filesystem::absolute(filepath).parent_path();
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
        std::unordered_map<uint32_t, entt::entity> loadedEntities;
        if (entities) {
            for (const auto& entityNode : entities) {
                std::string name = "Entity";
                const auto tagNode = entityNode["TagComponent"];
                if (tagNode) {name = tagNode["Tag"].as<std::string>();}
                Entity entity = m_Scene->CreateEntity(name);
                if (entityNode["Entity"]) {
                    const uint32_t savedID = entityNode["Entity"].as<uint32_t>();
                    if (!loadedEntities.emplace(savedID, static_cast<entt::entity>(entity)).second) {
                        SCIVIBE_CORE_ERROR("Deux entites portent le meme identifiant dans le fichier de scene.");
                        return false;
                    }
                }
                const auto transformNode = entityNode["TransformComponent"];
                if (transformNode) {auto& transform = entity.GetComponent<TransformComponent>();
                    transform.Translation = transformNode["Translation"].as<glm::vec3>();
                    transform.Rotation = transformNode["Rotation"].as<glm::vec3>();
                    transform.Scale = transformNode["Scale"].as<glm::vec3>();
                }
                if (entityNode["CameraComponent"]) {
                    const auto cameraComponentNode = entityNode["CameraComponent"];
                    const auto cameraNode = cameraComponentNode["Camera"];
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
                    const auto spriteRendererNode = entityNode["SpriteRendererComponent"];
                    glm::vec4 color = spriteRendererNode["Color"].as<glm::vec4>();
                    auto& sprite = entity.AddComponent<SpriteRendererComponent>(color);
                    sprite.Slides = ReadSlides(spriteRendererNode["Slides"]);
                    const float tiling = spriteRendererNode["TilingFactor"].as<float>(1.0f);
                    if (std::isfinite(tiling)) sprite.TilingFactor = std::clamp(tiling, 0.1f, 100.0f);
                    const auto storedPath = spriteRendererNode["TexturePath"].as<std::string>("");
                    if (!storedPath.empty()) {
                        auto imagePath = std::filesystem::path(storedPath);
                        if (imagePath.is_relative()) imagePath = sceneDirectory / imagePath;
                        sprite.TexturePath = imagePath.lexically_normal().string();
                        sprite.SetTexture(sprite.TexturePath);
                    }
                }
                if (auto node = entityNode["TextComponent"]) {
                    auto& text = entity.AddComponent<TextComponent>();
                    text.Source = node["Source"].as<std::string>("");
                    text.Color = node["Color"].as<glm::vec4>(glm::vec4(1.0f));
                    text.Slides = ReadSlides(node["Slides"]);
                    text.Visible = node["Visible"].as<bool>(true);
                    text.FontSize = node["FontSize"].as<float>(36.0f);
                    text.LineSpacing = node["LineSpacing"].as<float>(10.0f);
                    text.MaxWidth = node["MaxWidth"].as<int>(800);
                }
                if (auto node = entityNode["CartesianGridComponent"]) {
                    auto& grid = entity.AddComponent<CartesianGridComponent>();
                    grid.Slides = ReadSlides(node["Slides"]);
                    grid.Visible = node["Visible"].as<bool>(true);
                    if (auto cells = node["HalfCells"]; cells && cells.IsSequence() && cells.size() == 2) {
                        grid.HalfCells.x = std::clamp(cells[0].as<int>(), 1, 1000);
                        grid.HalfCells.y = std::clamp(cells[1].as<int>(), 1, 1000);
                    }
                    if (auto step = node["CellSize"]; step && step.IsSequence() && step.size() == 2) {
                        for (int i = 0; i < 2; ++i) {
                            const float value = step[i].as<float>();
                            if (std::isfinite(value)) grid.CellSize[i] = std::clamp(value, 0.001f, 1000.0f);
                        }
                    }
                    DeserializeCurveStyle(node, grid.Style);
                }
                if (auto node = entityNode["FunctionCurveComponent"]) {
                    auto& curve = entity.AddComponent<FunctionCurveComponent>();
                    curve.Slides = ReadSlides(node["Slides"]);
                    curve.Visible = node["Visible"].as<bool>(true);
                    curve.Expression = node["Expression"].as<std::string>("sin(x)");
                    curve.MinX = node["MinX"].as<double>(-5.0);
                    curve.MaxX = node["MaxX"].as<double>(5.0);
                    curve.Segments = node["Segments"].as<int>(512);
                    DeserializeCurveStyle(node, curve.Style);
                }
                if (auto node = entityNode["TransformAnimationComponent"]) {
                    auto& animation = entity.AddComponent<TransformAnimationComponent>();
                    animation.Start = animation.End = entity.GetComponent<TransformComponent>();
                    animation.Enabled = node["Enabled"].as<bool>(true);
                    animation.TriggerSlides = ReadSlides(node["TriggerSlides"], {1});
                    const float duration = node["Duration"].as<float>(1.0f);
                    if (std::isfinite(duration)) animation.Duration = std::clamp(duration, 0.0f, 3600.0f);
                    animation.Easing = static_cast<EasingType>(std::clamp(node["Easing"].as<int>(1), 0, 2));
                    animation.AnimatePosition = node["AnimatePosition"].as<bool>(true);
                    animation.AnimateRotation = node["AnimateRotation"].as<bool>(true);
                    animation.AnimateScale = node["AnimateScale"].as<bool>(true);
                    animation.AnimateColor = node["AnimateColor"].as<bool>(false);
                    animation.ColorTarget = static_cast<AnimationColorTarget>(std::clamp(node["ColorTarget"].as<int>(0), 0, 3));
                    animation.StartColor = node["StartColor"].as<glm::vec4>(glm::vec4(1.0f));
                    animation.EndColor = node["EndColor"].as<glm::vec4>(glm::vec4(1.0f));
                    DeserializePose(node["Start"], animation.Start);
                    DeserializePose(node["End"], animation.End);
                }
            }
        }
        // Resolve only after every entity exists. Saved EnTT handles are file
        // identifiers here, never handles to reuse directly in the new registry.
        if (entities) for (const auto& entityNode : entities) {
            const auto node = entityNode["TransformAnimationComponent"];
            if (!node || !entityNode["Entity"]) continue;
            const auto found = loadedEntities.find(entityNode["Entity"].as<uint32_t>());
            if (found == loadedEntities.end()) continue;
            auto& animation = m_Scene->m_Registry.get<TransformAnimationComponent>(found->second);
            DeserializeAnimationSource(node["StartTransformSource"], animation.StartTransformSource, loadedEntities);
            DeserializeAnimationSource(node["EndTransformSource"], animation.EndTransformSource, loadedEntities);
            DeserializeAnimationSource(node["StartColorSource"], animation.StartColorSource, loadedEntities);
            DeserializeAnimationSource(node["EndColorSource"], animation.EndColorSource, loadedEntities);
        }
        m_Scene->SetCurrentSlide(data["CurrentSlide"].as<int>(1));
        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string& filepath) {
        SCIVIBE_CORE_ASSERT(false, "Runtime deserialization is not supported yet.");
        return true;
    }
}
