#pragma once

#include "layer/layer.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Events/event.hpp"
#include "Events/keyEvent.hpp"
#include "Events/mouseEvent.hpp"
#include "Events/applicationEvent.hpp"



namespace scivibe{
    class SCIVIBE_API ImGuiLayer : public  Layer
    {
        public:
            ImGuiLayer();
            ~ImGuiLayer();

            
            void OnAttach() override ;
            void OnDetach() override;
            void OnUpdate() override;
            void OnEvent(Event& event);
        private:
            float m_time = 0.0f;
            bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& event);
            bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event); 
            bool OnMouseMovedEvent(MouseMovedEvent& event); 
            bool OnMouseScrolledEvent(MouseScrolledEvent& event);   
            bool OnKeyPressedEvent(KeyPressedEvent& event);
            bool OnKeyReleasedEvent(KeyReleasedEvent& event); 
            bool OnKeyTypedEvent(KeyTypedEvent& event);
            bool OnWindowResizeEvent(WindowResizeEvent &event); 
    };
}