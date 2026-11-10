#include "pch/pch.hpp"
#include "scene/sceneCamera.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace scivibe{
    SceneCamera::SceneCamera(){
        RecalculateProjection();
    }
    void SceneCamera::SetViewportSize(uint32_t width, uint32_t height){
        m_AspectRatio = (float) width / (float) height;
        RecalculateProjection();
    }
    void SceneCamera::SetOrthographic(float size, float nearClip, float farClip){
        m_ProjectionType = ProjectionType::Orthographic;
        m_OrthographicFar = farClip;
        m_OrthographicNear = nearClip;
        m_OrthographicSize = size;
        RecalculateProjection();
    }
    void SceneCamera::SetPerspective(float fov, float nearClip, float farClip){
        m_ProjectionType = ProjectionType::Perspective;
        m_PerspectiveFar = farClip;
        m_PerspectiveNear = nearClip;
        m_PerspectiveFOV = fov;
        RecalculateProjection();
    }
    void SceneCamera::RecalculateProjection(){
        if(m_ProjectionType == ProjectionType::Orthographic){
            float orthoLeft = -m_OrthographicSize*m_AspectRatio*0.5f;
            float orthoRight = m_OrthographicSize *m_AspectRatio*0.5;
            float orthoBottom = -m_OrthographicSize *0.5;
            float orthoTop = m_OrthographicSize *0.5;
            m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
        }else{
            m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
        }
    }
}