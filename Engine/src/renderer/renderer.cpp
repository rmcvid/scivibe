#include "pch/pch.hpp"
#include "renderer/renderer.hpp"
#include "plateform/OpenGl/OpenGLShader.hpp"
namespace scivibe{

    Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
    
    void Renderer::BeginScene(OrthographicCamera& camera){
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

    void Renderer::EndScene(){
    }    
    void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray> &vertexArray, const glm::mat4 transform ){
        shader->Bind();
        std::dynamic_pointer_cast<OpenGLShader>(shader)->setUniformMat4("uViewProjection",m_SceneData->ViewProjectionMatrix);
        std::dynamic_pointer_cast<OpenGLShader>(shader)->setUniformMat4("uTransform",transform);
        //mi.bind()
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

}
