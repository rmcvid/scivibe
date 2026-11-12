#pragma once
#include "scivibe.h"
#include <optional>
class Sandbox2D : public scivibe::Layer
{
    public:
        Sandbox2D();
        virtual ~Sandbox2D() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        
        void OnUpdate(scivibe::Timestep ts ) override ;
        virtual void OnImGuiRender() override;
        void OnEvent(scivibe::Event& e) override;
        void ImportText(); // à supprimer
    private:
        bool OnKeyPressed(scivibe::KeyPressedEvent& e);
        void NewScene();
        void SaveSceneAs();
        void OpenScene();
        void ResizeViewportToRecording();
        scivibe::OrthographicCameraController m_CameraController;
        scivibe::Ref<scivibe::VertexArray> m_VertexArrayBlue;
        scivibe::Ref<scivibe::Shader> m_FlatColorShader;
        scivibe::Ref<scivibe::Font> m_Font;
        scivibe::Ref<scivibe::Texture2D> m_TextureTest;
        
        scivibe::CurvePath m_CurvePath;
        scivibe::CurveStyle m_CurveStyle;
        
        scivibe::PsFile m_PsFile;
        scivibe::Line m_Line;
        std::optional<scivibe::Animator<scivibe::Object2D>> m_LineAnimation;
        std::optional<scivibe::Animator<scivibe::Repere>> m_RepereAnimation;
        
        scivibe::Ref<scivibe::Scene> m_ActiveScene;
        scivibe::Entity  m_SquareEntity;
        scivibe::Entity m_CameraEntity;
        scivibe::Entity m_SecondCamera;

        scivibe::EditorCamera m_EditorCamera;
        
        
        glm::vec2 m_ViewPortSize = {0,0};
        bool m_ViewPortFocused = false;
        bool m_ViewPortHovered = false;
        glm::uvec2 m_RenderResolution{1280, 720};
        bool m_ResizeViewportRequested = false;
        
        glm::vec4 m_Color = {0.7f,0.7f,0.7f,1.0f};
        int m_GuizmoType = -1;
        
        // pannels
        scivibe::SceneHierarchyPanel m_SceneHierarchyPanel;


};
