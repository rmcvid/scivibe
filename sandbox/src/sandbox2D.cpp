#include "sandbox2D.hpp"
#include <imgui.h>
#include "plateform/OpenGl/OpenGLShader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Sandbox2D::Sandbox2D()
    :Layer("SandBox2D") , m_CameraController(1280.0f/720.0f, true)
{

}
void Sandbox2D::OnAttach(){
    m_VertexArrayBlue = scivibe::VertexArray::Create();
    float squareVertices[4*3] {
        -0.5f,  -0.5f,  0.0f,
        0.5f,   -0.5f,  0.0f, 
        0.5f,   0.5f,   0.0f, 
        -0.5f,  0.5f,   0.0f
    };
    scivibe::Ref<scivibe::VertexBuffer> squareVertexBuffer;
    squareVertexBuffer.reset(scivibe::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
    scivibe::BufferLayout squareLayout {
        {scivibe::ShaderDataType::Float3, "aPosition"}
    };
    squareVertexBuffer->SetLayout(squareLayout);
    m_VertexArrayBlue->AddVertexBuffer(squareVertexBuffer);
    uint32_t squareIndices[6] = {0,1,2,2,3,0};

    scivibe::Ref<scivibe::IndexBuffer> squareBuffer;
    squareBuffer.reset(scivibe::IndexBuffer::Create(squareIndices, sizeof(squareIndices)/ sizeof(uint32_t)));
    m_VertexArrayBlue->SetIndexBuffer(squareBuffer);
    m_FlatColorShader = scivibe::Shader::Create( 
        SHADER_SANDBOX_PATH "FlatColor.glsl"
    );
}
void Sandbox2D::OnDetach(){

}
        
void Sandbox2D::OnUpdate(scivibe::Timestep ts ){
    m_CameraController.OnUpdate(ts);

    scivibe::RenderCommand::SetClearColor({0.0f,0.0f,0.0f,0.0f});
    scivibe::RenderCommand::Clear();
    scivibe::Renderer::BeginScene(m_CameraController.GetCamera());

    //glm::mat4 transform = glm::translate(glm::mat4(1.0f),m_squarePosition);
    std::dynamic_pointer_cast<scivibe::OpenGLShader>(m_FlatColorShader)->Bind();
    std::dynamic_pointer_cast<scivibe::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("uColor",m_Color);
    //m_FlatColorShader->Bind();
    scivibe::Renderer::Submit(m_FlatColorShader, m_VertexArrayBlue,  glm::scale(glm::mat4(1.0f),glm::vec3(1.5f)));
    scivibe::Renderer::EndScene();

}
void Sandbox2D::OnImGuiRender(){
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("square Color", glm::value_ptr(m_Color));
    ImGui::End();

};
void Sandbox2D::OnEvent(scivibe::Event& e){
    m_CameraController.OnEvent(e);
};
