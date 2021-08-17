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
        template<typename... Args>
        static void Info(const tchar* fmt, Args&&... args)
        {
            GetLogger()->info(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Warn(const tchar* fmt, Args&&... args)
        {
            GetLogger()->warn(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Error(const tchar* fmt, Args&&... args)
        {
            GetLogger()->error(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Fatal(const tchar* fmt, Args&&... args)
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

    #define PL_INFO(fmt, ...) {LogSystem::Info(fmt, ## __VA_ARGS__);}
    #define PL_WARN(fmt, ...) {LogSystem::Warn(fmt, ## __VA_ARGS__);}
    #define PL_ERROR(fmt, ...) {LogSystem::Error(fmt, ## __VA_ARGS__);}
    #define PL_FATAL(fmt, ...) {LogSystem::Fatal(fmt, ## __VA_ARGS__);}
}
