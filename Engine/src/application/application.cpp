#include "application/application.hpp"
#include "log/log.hpp"
#include "events/applicationEvent.hpp"

namespace scivibe {

    Application::Application() {
        SCIVIBE_CORE_INFO("Application created");
    }

    Application::~Application() {
        SCIVIBE_CORE_INFO("Application destroyed");
    }

    void Application::Run() {

        SCIVIBE_CORE_INFO("Application running...");
        WindowResizeEvent e(1280, 720);
        SCIVIBE_TRACE(e);

        while (true) {
            
        }
    }

} // namespace scivibe