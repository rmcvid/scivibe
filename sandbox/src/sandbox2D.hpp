#pragma once
#include "scivibe.h"

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

    private:
        scivibe::OrthographicCameraController m_CameraController;
        
        scivibe::Ref<scivibe::VertexArray> m_VertexArrayBlue;
        scivibe::Ref<scivibe::Shader> m_FlatColorShader;

        scivibe::Ref<scivibe::Texture2D> m_TextureTest;

        glm::vec4 m_Color = {0.7f,0.7f,0.7f,1.0f};


};
