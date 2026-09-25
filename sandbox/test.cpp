#include <scivibe.h>

class ExampleLayer : public scivibe::Layer
{
    public :
    ExampleLayer()
        : Layer("Example"),  m_Camera(-1.0f,1.0f,-1.0f,1.0f)

        {
            m_VertexArray.reset(scivibe::VertexArray::Create());
            float vertices[3*7] {
                -0.5f,-0.5f, 0.0f, 1.0f,1.0f, 1.0f ,1.0f,
                0.5f,-0.5f, 0.0f, 0.2f,0.3f,0.8f,1.0f,
                -0.0f, 0.5f, 0.0f, 0.1f,0.9f,0.2f,1.0f
            };
            std::shared_ptr<scivibe::VertexBuffer> vertexBuffer;
            vertexBuffer.reset(scivibe::VertexBuffer::Create(vertices, sizeof(vertices)));
            scivibe::BufferLayout layout {
                {scivibe::ShaderDataType::Float3, "aPosition"},
                {scivibe::ShaderDataType::Float4, "aColor"}
            };
            vertexBuffer->SetLayout(layout);
            m_VertexArray->AddVertexBuffer(vertexBuffer);

            uint32_t indices[3] = {0,1,2};
            std::shared_ptr<scivibe::IndexBuffer> indexBuffer;
            indexBuffer.reset(scivibe::IndexBuffer::Create(indices, sizeof(indices)/ sizeof(uint32_t)));
            m_VertexArray->SetIndexBuffer(indexBuffer);
            m_Shader.reset(new scivibe::Shader(
                SHADER_PATH "shader.vs",
                SHADER_PATH "shader.fs"
            ));
        }
    
        void OnUpdate(scivibe::Timestep ts) override{
            // à bouger dans une fonction move et remplacer par la direction de vue
            SCIVIBE_CORE_INFO("delta time {0}s ( {1}ms)", ts.GetSeconds(),ts.GetMiliseconds());
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
            m_Camera.setPosition(m_CameraPosition);
            m_Camera.setRotation(0);

            scivibe::RenderCommand::SetClearColor({0.0f,0.0f,0.0f,0.0f});
            scivibe::RenderCommand::Clear();
            scivibe::Renderer::BeginScene(m_Camera);
            scivibe::Renderer::Submit(m_Shader ,m_VertexArray);
            scivibe::Renderer::EndScene();
        }

        void OnEvent(scivibe::Event& event) override{
            //scivibe::EventDispatcher dispatcher(event);
            //dispatcher.Dispatch<scivibe::KeyPressedEvent>(SCIVIBE_BIND_EVENT_FN(onKeyPressedEvent));
            //SCIVIBE_TRACE("{0}",event);
        }
        bool onKeyPressedEvent(scivibe::KeyPressedEvent event){
            // on peut ici dire ce qu'on veut qu'il se passe
            return false;
        }
    private:
        std::shared_ptr<scivibe::Shader> m_Shader; 
        std::shared_ptr<scivibe::VertexArray> m_VertexArray;
        scivibe::OrthographicCamera m_Camera;
        glm::vec3 m_CameraPosition;
        float m_CameraSpeed = 20.0f;
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