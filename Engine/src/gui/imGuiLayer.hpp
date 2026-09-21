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
            void OnImGuiRender() override ;
            void Begin();
            void End();
        private:
            float m_time = 0.0f;
    };
}