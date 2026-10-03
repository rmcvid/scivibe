#pragma once
#include "renderer/camera.hpp"
#include "core/timeStep.hpp"
#include "Events/applicationEvent.hpp"
#include "Events/mouseEvent.hpp"

namespace scivibe{
    class SCIVIBE_API OrthographicCameraController{
        public:
            OrthographicCameraController(float aspectRatio, bool rotation = false);
            void OnUpdate(Timestep ts);
            void OnEvent(Event& event);
            bool OnMouseScrolled(MouseScrolledEvent& e);
            bool OnWindowResized(WindowResizeEvent& e);
            OrthographicCamera& GetCamera(){return m_Camera;}
            const OrthographicCamera& GetCamera() const {return m_Camera;}
            float GetCameraTranslationSpeed() const {return m_CameraTranslationSpeed;}
            void SetCameraTranslationSpeed(float speed) {m_CameraTranslationSpeed = speed;}
            float GetCameraRotationSpeed() const {return m_CameraRotationSpeed;}
            void SetCameraRotationSpeed(float speed) {m_CameraRotationSpeed = speed;}
            void SetZoomLevel(float level){ m_ZoomLevel = level;};
            float GetZoomLevel(){return m_ZoomLevel;}

        private:
            float m_ZoomLevel {1.0f};
            float m_AspectRatio;
            // Projection settings must be initialized before the camera uses them.
            OrthographicCamera m_Camera;
            bool m_Rotation;
            glm::vec3 m_CameraPosition {0.0f,0.0f,0.0f};
            float m_CameraRotation {0.0f};

            float m_CameraTranslationSpeed {5.0f};
            float m_CameraRotationSpeed {180.0f};


            
    };
}
