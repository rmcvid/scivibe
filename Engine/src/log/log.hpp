#pragma once
//#include "core.hpp"
#include "spdlog/spdlog.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include "spdlog/fmt/ostr.h"

namespace scivibe {
    class Log {
        private:
            static std::shared_ptr<spdlog::logger> s_CoreLogger;
            static std::shared_ptr<spdlog::logger> s_ClientLogger;
        public:
            static void Init();
            inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger;}
            inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger;}
            static void info(const char* message);
            static void warning(const char* message);
            static void error(const char* message);
    };
}

// core log macros
#define SCIVIBE_CORE_TRACE(...)    ::scivibe::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SCIVIBE_CORE_INFO(...)     ::scivibe::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SCIVIBE_CORE_WARN(...)     ::scivibe::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SCIVIBE_CORE_ERROR(...)    ::scivibe::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SCIVIBE_CORE_FATAL(...)    ::scivibe::Log::GetCoreLogger()->critical(__VA_ARGS__)
// client log macros
#define SCIVIBE_TRACE(...)         ::scivibe::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SCIVIBE_INFO(...)          ::scivibe::Log::GetClientLogger()->info(__VA_ARGS__)
#define SCIVIBE_WARN(...)          ::scivibe::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SCIVIBE_ERROR(...)         ::scivibe::Log::GetClientLogger()->error(__VA_ARGS__)
#define SCIVIBE_FATAL(...)         ::scivibe::Log::GetClientLogger()->critical(__VA_ARGS__)
