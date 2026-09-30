#include <scivibe.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "plateform/OpenGl/OpenGLShader.hpp"
#include <imgui.h>

class ExampleLayer : public scivibe::Layer
{
    public :
    ExampleLayer()
        : Layer("Example"),  m_Camera(-1.6f,1.6f,-0.9f,0.9f), m_squarePosition(0.0f)

        {
            m_VertexArray.reset(scivibe::VertexArray::Create());
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
            m_Shader.reset(scivibe::Shader::Create(
                SHADER_PATH "shader.vs",
                SHADER_PATH "shader.fs"
            ));


            m_VertexArrayBlue.reset(scivibe::VertexArray::Create());
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
            m_FlatColorShader.reset(scivibe::Shader::Create(
                SHADER_PATH "flatColorShader.vs",
                SHADER_PATH "flatColorShader.fs"
            ));
            
            //scivibe::Ref<scivibe::IndexBuffer> squareBuffer;
            //m_VertexArrayTexture->SetIndexBuffer(squareBuffer);
            m_TextureShader.reset(scivibe::Shader::Create(
                SHADER_PATH "textureShader.vs",
                SHADER_PATH "textureShader.fs"
            ));
            m_Texture = scivibe::Texture2D::Create("C:/Users/ryanm/Documents/Rmvi/scivibeVScode/sandbox/image/chess.png");
            std::dynamic_pointer_cast<scivibe::OpenGLShader>(m_TextureShader)->Bind();
            std::dynamic_pointer_cast<scivibe::OpenGLShader>(m_TextureShader)->UploadUniformInt("uTexture",0);
        }

        
    
        void OnUpdate(scivibe::Timestep ts) override{
            // à bouger dans une fonction move et remplacer par la direction de vue
            //SCIVIBE_CORE_INFO("delta time {0}s ( {1}ms)", ts.GetSeconds(),ts.GetMiliseconds());
            if(scivibe::Input::IsKeyPressed(SCIVIBE_KEY_LEFT)){
                m_CameraPosition.x -= ts * m_CameraSpeed;
            }
            if(scivibe::Input::IsKeyPressed(SCIVIBE_KEY_RIGHT)){
                m_CameraPosition.x += ts* m_CameraSpeed;
            }
            if(scivibe::Input::IsKeyPressed(SCIVIBE_KEY_DOWN)){
                m_CameraPosition.y -= ts*m_CameraSpeed;
            }
            if( scivibe::Input::IsKeyPressed(SCIVIBE_KEY_UP)){
                m_CameraPosition.y += ts * m_CameraSpeed;
            }

            if(scivibe::Input::IsKeyPressed(SCIVIBE_KEY_J)){
                m_squarePosition.x -= ts * m_CameraSpeed;
            }
            if(scivibe::Input::IsKeyPressed(SCIVIBE_KEY_L)){
                m_squarePosition.x += ts* m_CameraSpeed;
            }
            if(scivibe::Input::IsKeyPressed(SCIVIBE_KEY_I)){
                m_squarePosition.y -= ts*m_CameraSpeed;
            }
            if( scivibe::Input::IsKeyPressed(SCIVIBE_KEY_K)){
                m_squarePosition.y += ts * m_CameraSpeed;
            }
            
            m_Camera.setPosition(m_CameraPosition);
            m_Camera.setRotation(0);


            scivibe::RenderCommand::SetClearColor({0.0f,0.0f,0.0f,0.0f});
            scivibe::RenderCommand::Clear();
            scivibe::Renderer::BeginScene(m_Camera);

            //scivibe::MaterialRef material = new scivibe::Material(m_FlatColorShader);
            //scivibe::MaterialInstanceRef mi = new scivibe::MaterialInstance(material);
            //mi->Set("uColor",uColor);
            //material->Set("uColor",uColor);
            glm::mat4 transform = glm::translate(glm::mat4(1.0f),m_squarePosition);
            std::dynamic_pointer_cast<scivibe::OpenGLShader>(m_FlatColorShader)->Bind();
            std::dynamic_pointer_cast<scivibe::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("uColor",m_Color);
            //civibe::Renderer::Submit(m_FlatColorShader ,m_VertexArrayBlue, transform);
            m_Texture->Bind(0);
            scivibe::Renderer::Submit(m_TextureShader ,m_VertexArrayBlue,  glm::scale(glm::mat4(1.0f),glm::vec3(1.5f)));
            scivibe::Renderer::EndScene();
        }

        void OnEvent(scivibe::Event& event) override{
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
        scivibe::Ref<scivibe::Shader> m_TextureShader; 




        scivibe::OrthographicCamera m_Camera;

        glm::vec3 m_CameraPosition;
        float m_CameraSpeed = 20.0f;
        float m_CameraRotation = 0.0f;
        float m_CameraRotationSpeed = 180.0f;

        glm::vec3 m_squarePosition;

        glm::vec4 m_Color = {0.7f,0.7f,0.7f,1.0f};

};

class Sandbox : public scivibe::Application{
    public:
        Sandbox(){
            PushLayer(new ExampleLayer());
        }
        ~Sandbox(){}
};

scivibe::Application* scivibe::CreateApplication(){
    return new Sandbox();
}



/*
int main(){
    scivibe::Log::Init();
    scivibe::Application app;
    app.Run();
    return 0;

}*/
