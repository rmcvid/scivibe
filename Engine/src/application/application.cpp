#include "application/application.hpp"
#include "log/log.hpp"
#include "Events/applicationEvent.hpp"
#include "pch/pch.hpp"
#include "GLFW/glfw3.h"

namespace scivibe {

    Application::Application() {
        m_window = std::unique_ptr<Window>(Window::Create());
        SCIVIBE_CORE_INFO("Application created");
    }

    Application::~Application() {
        SCIVIBE_CORE_INFO("Application destroyed");
    }
    void Application::Run() {
        SCIVIBE_CORE_INFO("Application running...");
        WindowResizeEvent e(1280, 720);
        SCIVIBE_TRACE(e);

        while (m_Running) {
            //glClearColor(1,0,1,1);
            //glClear(GL_COLOR_BUFFER_BIT);
            m_window->onUpdate();
        }
    }

} // namespace scivibe
