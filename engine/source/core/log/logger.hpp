#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "core_minimal_private.hpp"
#include "foundation/smart_ptr.hpp"

namespace Engine
{
    class CORE_API LogSystem
    {
    public:
        template<typename TChar, typename... Args>
        static void Info(const TChar* category, const TChar* fmt, Args&&... args)
        {
            GetLogger()->info(fmt, std::forward<Args>(args)...);
        }

        template<typename TChar, typename... Args>
        static void Warn(const TChar* category, const TChar* fmt, Args&&... args)
        {
            GetLogger()->warn(fmt, std::forward<Args>(args)...);
        }

        template<typename TChar, typename... Args>
        static void Error(const TChar* category, const TChar* fmt, Args&&... args)
        {
            GetLogger()->error(fmt, std::forward<Args>(args)...);
        }

        template<typename TChar, typename... Args>
        static void Fatal(const TChar* category, const TChar* fmt, Args&&... args)
        {
            GetLogger()->critical(fmt, std::forward<Args>(args)...);
        }

    private:
        static SharedPtr<spdlog::logger> GetLogger()
        {
            static auto logger = spdlog::basic_logger_mt("engine_log", "logs/engine_log.txt");
            return logger;
        }
    };

    #define PL_INFO(category, fmt, ...) {LogSystem::Info(category, fmt, ## __VA_ARGS__);}
    #define PL_WARN(category, fmt, ...) {LogSystem::Warn(category, fmt, ## __VA_ARGS__);}
    #define PL_ERROR(category, fmt, ...) {LogSystem::Error(category, fmt, ## __VA_ARGS__);}
    #define PL_FATAL(category, fmt, ...) {LogSystem::Fatal(category, fmt, ## __VA_ARGS__);}
}
