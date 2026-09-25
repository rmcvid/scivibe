#pragma once
#include "core/core.hpp"
#include <glm/glm.hpp>
namespace scivibe{
    class SCIVIBE_API OrthographicCamera{
        public:
            OrthographicCamera(float left, float right, float bottom, float top);
            const glm::vec3& GetPosition(){return m_Position ;}
            void setPosition(const glm::vec3& position){m_Position = position;RecalculateViewMatrix();}
            
            const float GetRotation(){return m_Rotation ;}
            void setRotation(float rotation){m_Rotation = rotation;RecalculateViewMatrix();}

            const glm::mat4& GetProjectionMatrix(){return m_ProjectionMatrix;}
            const glm::mat4& GetViewMatrix(){return m_ViewMatrix;}
            const glm::mat4& GetViewProjectionMatrix(){return m_ViewProjectionMatrix;}
        
        
        private:
            void RecalculateViewMatrix();

            glm::mat4 m_ProjectionMatrix;
            glm::mat4 m_ViewMatrix;
            glm::mat4 m_ViewProjectionMatrix;
            glm::vec3 m_Position {0.0f,0.0f,0.0f};
            float m_Rotation=0.0f;
    };
}
