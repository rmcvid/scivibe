#pragma once
#include "core/core.hpp"
#include <memory>
#include <type_traits>
#include <utility>
#include "spdlog/spdlog.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include "spdlog/fmt/ostr.h"

namespace scivibe {
    class SCIVIBE_API Log {
        private:
            static std::shared_ptr<spdlog::logger> s_CoreLogger;
            static std::shared_ptr<spdlog::logger> s_ClientLogger;

            template<typename T>
            static decltype(auto) LogArgument(T&& value) {
                using Type = std::decay_t<T>;
                if constexpr (std::is_same_v<Type, const unsigned char*> ||
                              std::is_same_v<Type, unsigned char*>) {
                    // OpenGL returns zero-terminated text as GLubyte*.
                    const auto* text = value;
                    return text ? reinterpret_cast<const char*>(text) : "(null)";
                } else {
                    return std::forward<T>(value);
                }
            }

            // Check the format against the types actually passed to spdlog.
            template<typename... Args>
            using FormatString = spdlog::format_string_t<
                decltype(LogArgument(std::declval<Args>()))...>;

        public:
            static void Init();
            inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger;}
            inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger;}
            static void info(const char* message);
            static void warning(const char* message);
            static void error(const char* message);

            template<typename... Args>
            static void Write(const std::shared_ptr<spdlog::logger>& logger,
                              spdlog::level::level_enum level,
                              FormatString<Args...> format, Args&&... args) {
                logger->log(level, format, LogArgument(std::forward<Args>(args))...);
            }

            // A single message or event is data, even if it contains braces.
            template<typename T>
            static void Write(const std::shared_ptr<spdlog::logger>& logger,
                              spdlog::level::level_enum level, const T& message) {
                logger->log(level, "{}", LogArgument(message));
            }

            template<typename... Args>
            static void info(FormatString<Args...> format, Args&&... args) {
                Write(s_CoreLogger, spdlog::level::info, format, std::forward<Args>(args)...);
            }

            template<typename... Args>
            static void warning(FormatString<Args...> format, Args&&... args) {
                Write(s_CoreLogger, spdlog::level::warn, format, std::forward<Args>(args)...);
            }

            template<typename... Args>
            static void error(FormatString<Args...> format, Args&&... args) {
                Write(s_CoreLogger, spdlog::level::err, format, std::forward<Args>(args)...);
            }
    };
}

// core log macros
#define SCIVIBE_CORE_TRACE(...)    ::scivibe::Log::Write(::scivibe::Log::GetCoreLogger(), spdlog::level::trace, __VA_ARGS__)
#define SCIVIBE_CORE_INFO(...)     ::scivibe::Log::Write(::scivibe::Log::GetCoreLogger(), spdlog::level::info, __VA_ARGS__)
#define SCIVIBE_CORE_WARN(...)     ::scivibe::Log::Write(::scivibe::Log::GetCoreLogger(), spdlog::level::warn, __VA_ARGS__)
#define SCIVIBE_CORE_ERROR(...)    ::scivibe::Log::Write(::scivibe::Log::GetCoreLogger(), spdlog::level::err, __VA_ARGS__)
#define SCIVIBE_CORE_FATAL(...)    ::scivibe::Log::Write(::scivibe::Log::GetCoreLogger(), spdlog::level::critical, __VA_ARGS__)
// client log macros
#define SCIVIBE_TRACE(...)         ::scivibe::Log::Write(::scivibe::Log::GetClientLogger(), spdlog::level::trace, __VA_ARGS__)
#define SCIVIBE_INFO(...)          ::scivibe::Log::Write(::scivibe::Log::GetClientLogger(), spdlog::level::info, __VA_ARGS__)
#define SCIVIBE_WARN(...)          ::scivibe::Log::Write(::scivibe::Log::GetClientLogger(), spdlog::level::warn, __VA_ARGS__)
#define SCIVIBE_ERROR(...)         ::scivibe::Log::Write(::scivibe::Log::GetClientLogger(), spdlog::level::err, __VA_ARGS__)
#define SCIVIBE_FATAL(...)         ::scivibe::Log::Write(::scivibe::Log::GetClientLogger(), spdlog::level::critical, __VA_ARGS__)
