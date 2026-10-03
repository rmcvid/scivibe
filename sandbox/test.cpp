#include <scivibe.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "plateform/OpenGl/OpenGLShader.hpp"
#include <imgui.h>
#include "sandbox2D.hpp"
#include "core/entryPoint.hpp"


class ExampleLayer : public scivibe::Layer
{
    public :
    ExampleLayer()
        : Layer("Example"),  m_CameraController(1280.0f/720.0f, true), m_squarePosition(0.0f)
        {
            m_VertexArray = scivibe::VertexArray::Create();
            float vertices[3*7] {
                -0.5f,-0.5f, 0.0f, 1.0f,1.0f, 1.0f ,1.0f,
                0.5f,-0.5f, 0.0f, 0.2f,0.3f,0.8f,1.0f,
                -0.0f, 0.5f, 0.0f, 0.1f,0.9f,0.2f,1.0f
            };
            scivibe::Ref<scivibe::VertexBuffer> vertexBuffer;
            vertexBuffer.reset(scivibe::VertexBuffer::Create(vertices, sizeof(vertices)));
            scivibe::BufferLayout layout {
                {scivibe::ShaderDataType::Float3, "aPosition"},
                {scivibe::ShaderDataType::Float4, "aColor"}
            };
            vertexBuffer->SetLayout(layout);
            m_VertexArray->AddVertexBuffer(vertexBuffer);
            uint32_t indices[3] = {0,1,2};
            scivibe::Ref<scivibe::IndexBuffer> indexBuffer;
            indexBuffer.reset(scivibe::IndexBuffer::Create(indices, sizeof(indices)/ sizeof(uint32_t)));
            m_VertexArray->SetIndexBuffer(indexBuffer);
            m_Shader =scivibe::Shader::Create(
                "shader",
                SHADER_PATH "shader.vs",
                SHADER_PATH "shader.fs"
            );
            m_VertexArrayBlue = scivibe::VertexArray::Create();
            float squareVertices[4*5] {
                -0.5f,-0.5f, 0.0f, 0.0f,0.0f,
                 0.5f,-0.5f, 0.0f, 1.0f,0.0f,
                 0.5f, 0.5f, 0.0f, 1.0f,1.0f,
                -0.5f, 0.5f, 0.0f, 0.0f,1.0f
            };
            scivibe::Ref<scivibe::VertexBuffer> squareVertexBuffer;
            squareVertexBuffer.reset(scivibe::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
            scivibe::BufferLayout squareLayout {
                {scivibe::ShaderDataType::Float3, "aPosition"},
                {scivibe::ShaderDataType::Float2, "aTexCoord"}
            };
            squareVertexBuffer->SetLayout(squareLayout);
            m_VertexArrayBlue->AddVertexBuffer(squareVertexBuffer);
            uint32_t squareIndices[6] = {0,1,2,2,3,0};

            scivibe::Ref<scivibe::IndexBuffer> squareBuffer;
            squareBuffer.reset(scivibe::IndexBuffer::Create(squareIndices, sizeof(squareIndices)/ sizeof(uint32_t)));
            m_VertexArrayBlue->SetIndexBuffer(squareBuffer);
            m_FlatColorShader = scivibe::Shader::Create( 
                "flat",
                SHADER_PATH "flatColorShader.vs",
                SHADER_PATH "flatColorShader.fs"
            );
            
            auto textureShader = m_ShaderLib.Load(
                "Texture",
                SHADER_PATH "textureShader.vs",
                SHADER_PATH "textureShader.fs"
            );
            m_Texture = scivibe::Texture2D::Create( IMAGE_SANDBOX_PATH "chess.png");
            m_ScivibeLogo = scivibe::Texture2D::Create(IMAGE_SANDBOX_PATH "logo_transparent.png");
            std::dynamic_pointer_cast<scivibe::OpenGLShader>(textureShader)->Bind();
            std::dynamic_pointer_cast<scivibe::OpenGLShader>(textureShader)->UploadUniformInt("uTexture",0);
        }
        void OnUpdate(scivibe::Timestep ts) override{
            m_CameraController.OnUpdate(ts);
            // à bouger dans une fonction move et remplacer par la direction de vue
            //SCIVIBE_CORE_INFO("delta time {0}s ( {1}ms)", ts.GetSeconds(),ts.GetMiliseconds());

            if(scivibe::Input::IsKeyPressed(SCIVIBE_KEY_J)){
                m_squarePosition.x -= ts * m_CameraController.GetCameraTranslationSpeed();
            }
            if(scivibe::Input::IsKeyPressed(SCIVIBE_KEY_L)){
                m_squarePosition.x += ts* m_CameraController.GetCameraTranslationSpeed();
            }
            if(scivibe::Input::IsKeyPressed(SCIVIBE_KEY_I)){
                m_squarePosition.y -= ts*m_CameraController.GetCameraTranslationSpeed();
            }
            if( scivibe::Input::IsKeyPressed(SCIVIBE_KEY_K)){
                m_squarePosition.y += ts * m_CameraController.GetCameraTranslationSpeed();
            }
            
            scivibe::RenderCommand::SetClearColor({0.0f,0.0f,0.0f,0.0f});
            scivibe::RenderCommand::Clear();
            scivibe::Renderer::BeginScene(m_CameraController.GetCamera());

            glm::mat4 transform = glm::translate(glm::mat4(1.0f),m_squarePosition);
            std::dynamic_pointer_cast<scivibe::OpenGLShader>(m_FlatColorShader)->Bind();
            std::dynamic_pointer_cast<scivibe::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("uColor",m_Color);
            //civibe::Renderer::Submit(m_FlatColorShader ,m_VertexArrayBlue, transform);
            auto textureShader = m_ShaderLib.Get("Texture");
            m_Texture->Bind(0);
            scivibe::Renderer::Submit(textureShader, m_VertexArrayBlue,  glm::scale(glm::mat4(1.0f),glm::vec3(1.5f)));

            m_ScivibeLogo->Bind(0);
            scivibe::Renderer::Submit(textureShader, m_VertexArrayBlue,  glm::scale(glm::mat4(1.0f),glm::vec3(1.5f)));


            scivibe::Renderer::EndScene();
        }

        void OnEvent(scivibe::Event& event) override{
            m_CameraController.OnEvent(event);
            //scivibe::EventDispatcher dispatcher(event);
            //dispatcher.Dispatch<scivibe::KeyPressedEvent>(SCIVIBE_BIND_EVENT_FN(onKeyPressedEvent));
            //SCIVIBE_TRACE("{0}",event);
        }
        virtual void OnImGuiRender() override{
            ImGui::Begin("Settings");
            ImGui::ColorEdit3("square Color", glm::value_ptr(m_Color));
            ImGui::End();
            
        }

        bool onKeyPressedEvent(scivibe::KeyPressedEvent event){
            // on peut ici dire ce qu'on veut qu'il se passe
            return false;
        }
    private:
        scivibe::Ref<scivibe::Shader> m_Shader;
        scivibe::Ref<scivibe::VertexArray> m_VertexArray;
            
        scivibe::Ref<scivibe::Shader> m_FlatColorShader; 
        scivibe::Ref<scivibe::VertexArray> m_VertexArrayBlue;

        scivibe::Ref<scivibe::Texture2D> m_Texture;
        scivibe::Ref<scivibe::Texture2D> m_ScivibeLogo;
        scivibe::ShaderLibrary m_ShaderLib;

        scivibe::OrthographicCameraController m_CameraController;
        glm::vec3 m_CameraPosition;
        glm::vec3 m_squarePosition;

        glm::vec4 m_Color = {0.7f,0.7f,0.7f,1.0f};

};

class Sandbox : public scivibe::Application{
    public:
        Sandbox(){
            //PushLayer(new ExampleLayer());
            PushLayer(new Sandbox2D());

        }
        ~Sandbox(){}
};

scivibe::Application* scivibe::CreateApplication(){
    return new Sandbox();
}
