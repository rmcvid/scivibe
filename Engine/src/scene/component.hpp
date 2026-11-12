#pragma once
#include "pch/pch.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/camera.hpp"
#include "scene/sceneCamera.hpp"
#include "scene/scriptableEntity.hpp"
#include "renderer/rendererCurve2D.hpp"
#include "renderer/texture.hpp"
#include "animator/easing.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

namespace scivibe{
    struct TransformComponent{
        glm::vec3 Translation = {0.0f,0.0f,0.0f};
        glm::vec3 Rotation = {0.0f,0.0f,0.0f};
        glm::vec3 Scale = {1.0f, 1.0f, 1.0f};
        TransformComponent()= default;
        TransformComponent(const TransformComponent& ) = default;
        TransformComponent(const glm::vec3& translation)
            : Translation(translation){}
        glm::mat4 GetTransform() const{
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
            return glm::translate(glm::mat4(1.0f),Translation) * rotation * glm::scale(glm::mat4(1.0f),Scale);
        }
    };
    
    struct SpriteRendererComponent{
        glm::vec4 Color{1.0f,1.0f,1.0f,1.0f};
        Ref<Texture2D> Texture;
        std::string TexturePath, TextureError;
        float TilingFactor = 1.0f;
        std::vector<int> Slides; // Empty means visible on every slide.
        SCIVIBE_API bool SetTexture(const std::string& path);
        //Ref<MaterialInstance> Material; 
        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent& ) = default;
        SpriteRendererComponent(const glm::vec4& color): Color(color){};
    };
    enum class AnimationColorTarget { Sprite, Text, Grid, Function };
    struct AnimationValueSource {
        bool UseEntity = false;
        entt::entity Entity = entt::null;
        AnimationColorTarget ColorTarget = AnimationColorTarget::Sprite;
    };

    struct TransformAnimationComponent {
        bool Enabled = true;
        std::vector<int> TriggerSlides{1};
        TransformComponent Start, End;
        float Duration = 1.0f;
        EasingType Easing = EasingType::Smoothstep;
        bool AnimatePosition = true, AnimateRotation = true, AnimateScale = true;
        bool AnimateColor = false;
        AnimationColorTarget ColorTarget = AnimationColorTarget::Sprite;
        glm::vec4 StartColor{1.0f}, EndColor{1.0f};
        AnimationValueSource StartTransformSource, EndTransformSource;
        AnimationValueSource StartColorSource, EndColorSource;

        // Preview state is deliberately not serialized into the scene.
        int LastSlide = 0;
        double Elapsed = 0.0;
        bool HasPose = false;
        bool Running = false;
        TransformComponent Pose;
        glm::vec4 Color{1.0f};
        TransformComponent CapturedStart, CapturedEnd;
        glm::vec4 CapturedStartColor{1.0f}, CapturedEndColor{1.0f};
        std::string ReferenceError, StartReferenceError;
    };

    struct TagComponent{
        std::string Tag;
        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag): Tag(tag){
        }
    };

    struct CameraComponent{
        SceneCamera Camera;
        bool Primary = true;
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent& ) = default;
    };

    struct NativeScriptComponent{
        ScriptableEntity* Instance = nullptr;
        ScriptableEntity*(*InstantiateScript)();
        void (*DestroyScript)(NativeScriptComponent*);
        template<typename T>
        void Bind(){
            InstantiateScript = [](){ return static_cast<ScriptableEntity*>(new T()); };
            DestroyScript = [](NativeScriptComponent* nsc){ delete nsc->Instance; nsc->Instance = nullptr; };
        }
    };
    
    struct TextComponent{
        std::string Source;
        bool Visible = true;
        float FontSize = 36.0f;
        float LineSpacing = 10.0f;
        int MaxWidth = 800;
        std::vector<int> Slides;
        glm::vec4 Color{1.0f};
    };

    struct CartesianGridComponent {
        bool Visible = true;
        glm::ivec2 HalfCells{5, 5};
        glm::vec2 CellSize{1.0f, 1.0f};
        CurveStyle Style{glm::vec4(0.35f, 0.35f, 0.35f, 1.0f), 1.0f};
        std::vector<int> Slides;
    };

    struct FunctionCurveComponent {
        std::vector<int> Slides;
        bool Visible = true;
        std::string Expression = "sin(x)";
        double MinX = -5.0, MaxX = 5.0;
        int Segments = 512;
        CurveStyle Style{glm::vec4(0.2f, 0.8f, 1.0f, 1.0f), 3.0f};

        SCIVIBE_API const CurvePath& GetPath();
        const std::string& GetError() const { return m_Error; }
    private:
        bool m_Cached = false;
        std::string m_CachedExpression, m_Error;
        double m_CachedMinX = 0.0, m_CachedMaxX = 0.0;
        int m_CachedSegments = 0;
        CurvePath m_Path;
    };
}
