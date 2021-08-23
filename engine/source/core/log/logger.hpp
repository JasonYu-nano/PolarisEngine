#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
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

        template<typename TChar, typename... Args>
        static void Log(spdlog::source_loc src, spdlog::level::level_enum level, const TChar* category, const TChar* fmt, Args&&... args)
        {
            GetLogger()->log(src, level, fmt, std::forward<Args>(args)...);
        }

    private:
        static SharedPtr<spdlog::logger> GetLogger()
        {
            static LogSystem system;
            return system.Logger;
        }

        LogSystem()
        {
            std::vector<spdlog::sink_ptr> sinks;
            auto colorSink = MakeSharedPtr<spdlog::sinks::stdout_color_sink_mt>();
            colorSink->set_color(spdlog::level::info, colorSink->WHITE);
            sinks.push_back(colorSink);
            sinks.push_back(MakeSharedPtr<spdlog::sinks::basic_file_sink_mt>("engine_log", "logs/engine_log.txt"));
            Logger = MakeSharedPtr<spdlog::logger>("engine_log", begin(sinks), end(sinks));
            Logger->set_pattern("%^[%H:%M:%S] [%s:%#] [%!] %v%$");
        }

        SharedPtr<spdlog::logger> Logger;
    };

    #define PL_LOGIMPL(level, category, fmt, ...)  {LogSystem::Log(spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, level, category, fmt, ## __VA_ARGS__);}
    #define PL_INFO(category, fmt, ...) PL_LOGIMPL(spdlog::level::info, category, fmt, ## __VA_ARGS__)
    #define PL_WARN(category, fmt, ...) PL_LOGIMPL(spdlog::level::warn, category, fmt, ## __VA_ARGS__)
    #define PL_ERROR(category, fmt, ...) PL_LOGIMPL(spdlog::level::err, category, fmt, ## __VA_ARGS__)
    #define PL_FATAL(category, fmt, ...) PL_LOGIMPL(spdlog::level::critical, category, fmt, ## __VA_ARGS__)
}
