#include "pch/pch.hpp"
#include "renderer/renderer.hpp"
namespace scivibe{

    Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
    
    void Renderer::BeginScene(OrthographicCamera& camera){
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

    void Renderer::EndScene(){
    }    
    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray> &vertexArray){
        shader->Bind();
        shader->setUniformMat4("uViewProjection",m_SceneData->ViewProjectionMatrix);
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

}
