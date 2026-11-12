#include "pch/pch.hpp"
#include "editor/editorCamera.hpp"

#include "GLFW/glfw3.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "input/input.hpp"
#include <algorithm>
#include <cmath>

namespace scivibe{
    EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip)),
          m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
    {
        UpdateView();
    }

    void EditorCamera::SetViewportSize(float width, float height){
        if (!std::isfinite(width) || !std::isfinite(height) || width <= 0.0f || height <= 0.0f)
            return;
        m_ViewportWidth = width;
        m_ViewportHeight = height;
        UpdateProjection();
    }

    void EditorCamera::UpdateProjection(){
        m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
        m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
    }
    void EditorCamera::UpdateView(){
        m_Position = CalculatePosition();
        glm::quat orientation = GetOrientation();
        m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
        m_ViewMatrix = glm::inverse(m_ViewMatrix);
    }

    std::pair<float,float> EditorCamera::PanSpeed() const{
        float x = std::min(m_ViewportWidth / 1000.0f, 2.4f);
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;
        float y = std::min(m_ViewportHeight / 1000.0f, 2.4f);
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;
        return {xFactor, yFactor};
    }
    float EditorCamera::RotationSpeed() const{
        return 0.8f;
    }
    float EditorCamera::ZoomSpeed() const{
        float distance = m_Distance * 0.2f;
        distance = std::max(distance, 0.0f);
        float speed = distance * distance;
        speed = std::min(speed, 100.0f);
        return speed;
    }

    void EditorCamera::OnUpdate(Timestep ts, bool inputEnabled){
        const auto [mouseX, mouseY] = Input::GetMousePosition();
        const glm::vec2 mouse{mouseX, mouseY};
        const bool interacting = inputEnabled && Input::IsMouseButtonPressed(SCIVIBE_MOUSE_BUTTON_MIDDLE);
        // Starting or resuming a drag must not reuse a position from another panel.
        const glm::vec2 delta = interacting && m_MouseInteractionActive
            ? (mouse - m_InitialMousePosition) * 0.003f : glm::vec2(0.0f);
        m_InitialMousePosition = mouse;
        m_MouseInteractionActive = interacting;
        if(interacting){

            if(Input::IsMouseButtonPressed(SCIVIBE_MOUSE_BUTTON_LEFT))
                MouseRotate(delta);
            else
                MousePan(delta);
        }
        UpdateView();
    }

    void EditorCamera::OnEvent(Event& e){
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(SCIVIBE_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
    }

    bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e){
        float delta = e.GetYOffset() * 0.1f;
        MouseZoom(delta);
        UpdateView();
        return false;
    }

    void EditorCamera::MousePan(const glm::vec2& delta){
        auto [xSpeed, ySpeed] = PanSpeed();
        m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
        m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
    }

    void EditorCamera::MouseRotate(const glm::vec2& delta){
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        m_Yaw += yawSign * delta.x * RotationSpeed();
        m_Pitch += delta.y * RotationSpeed();
    }

    void EditorCamera::MouseZoom(float delta){
        m_Distance -= delta * ZoomSpeed();
        if(m_Distance < 1.0f){
            m_FocalPoint += GetForwardDirection();
            m_Distance = 1.0f;
        }
    }

    glm::vec3 EditorCamera::GetUpDirection() const{
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetRightDirection() const{
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetForwardDirection() const{
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 EditorCamera::CalculatePosition() const{
        return m_FocalPoint - GetForwardDirection() * m_Distance;
    }

    glm::quat EditorCamera::GetOrientation() const{
        return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
    }
}
