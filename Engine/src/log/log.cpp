#include "log/log.hpp"

namespace scivibe {
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
    void Log::Init() {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        s_CoreLogger = spdlog::stdout_color_mt("SCIVIBE");
        s_ClientLogger = spdlog::stdout_color_mt("APP");
        s_CoreLogger->set_level(spdlog::level::trace);
        s_ClientLogger->set_level(spdlog::level::trace);
    }

    void Log::info(const char* message) {
        Write(s_CoreLogger, spdlog::level::info, message);
    }

    void Log::warning(const char* message) {
        Write(s_CoreLogger, spdlog::level::warn, message);
    }

    void Log::error(const char* message) {
        Write(s_CoreLogger, spdlog::level::err, message);
    }
}
