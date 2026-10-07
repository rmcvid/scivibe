#include "pch/pch.hpp"
#include "renderer/renderer.hpp"
#include "plateform/OpenGl/OpenGLShader.hpp"
#include "renderer/renderer2D.hpp"
namespace scivibe{

    Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
    
    void Renderer::Init(){
        RenderCommand::Init();
        Renderer2D::Init();
    }
    void Renderer::BeginScene(OrthographicCamera& camera){
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }
    void Renderer::OnWindowResize(uint32_t width, uint32_t height){
        RenderCommand::SetViewport(0,0,width,height);
    }

    void Renderer::EndScene(){
    }    
    void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray> &vertexArray, const glm::mat4 transform ){
        shader->Bind();
        shader->SetMat4("uViewProjection",m_SceneData->ViewProjectionMatrix);
        shader->SetMat4("uTransform",transform);
        //mi.bind()
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

}
