#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/pattern_formatter.h"
#include "predefine/platform.hpp"
#include "foundation/smart_ptr.hpp"

namespace Engine
{
    enum ELogLevel
    {
        Trace = SPDLOG_LEVEL_TRACE,
        Debug = SPDLOG_LEVEL_DEBUG,
        Info = SPDLOG_LEVEL_INFO,
        Warn = SPDLOG_LEVEL_WARN,
        Error = SPDLOG_LEVEL_ERROR,
        Critical = SPDLOG_LEVEL_CRITICAL,
        OFF = SPDLOG_LEVEL_OFF,
    };

    class CORE_API LogSystem
    {
    public:
        template<typename TChar, typename... Args>
        static void Info(const ansi* category, const TChar* fmt, Args&&... args)
        {
            GetLogger()->info(fmt, std::forward<Args>(args)...);
        }

        template<typename TChar, typename... Args>
        static void Info(const char* category, const char16_t* fmt, Args&&... args)
        {
            GetLogger()->info(fmt, std::forward<Args>(args)...);
        }

        template<typename TChar, typename... Args>
        static void Warn(const ansi* category, const TChar* fmt, Args&&... args)
        {
            GetLogger()->warn(fmt, std::forward<Args>(args)...);
        }

        template<typename TChar, typename... Args>
        static void Error(const ansi* category, const TChar* fmt, Args&&... args)
        {
            GetLogger()->error(fmt, std::forward<Args>(args)...);
        }

        template<typename TChar, typename... Args>
        static void Fatal(const ansi* category, const TChar* fmt, Args&&... args)
        {
            GetLogger()->critical(fmt, std::forward<Args>(args)...);
        }

        template<typename TChar, typename... Args>
        static void Log(spdlog::source_loc src, spdlog::level::level_enum level, const ansi* category, const TChar* fmt, Args&&... args)
        {
            GetLogger()->log(src, level, fmt, std::forward<Args>(args)...);
        }

        template<typename TChar, typename... Args>
        static void Log(spdlog::source_loc src, ELogLevel level, const ansi* category, const TChar* fmt, Args&&... args)
        {
            GetLogger()->log(src, (spdlog::level::level_enum)level, fmt, std::forward<Args>(args)...);
        }

        template<typename TChar, typename... Args>
        static void Log(spdlog::level::level_enum level, const ansi* category, const TChar* fmt, Args&&... args)
        {
            GetLogger()->log(level, fmt, std::forward<Args>(args)...);
        }

        static void AssertFail(spdlog::source_loc src, const ansi* expr)
        {
            GetLogger()->log(src, spdlog::level::critical, "assertion failed: at {0}", expr);
            abort();
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
            //TODO: has bug with overriden operator new / delete
            //Logger->set_pattern("%^[%H:%M:%S] [%s:%#] [%!] %v%$");
        }

        SharedPtr<spdlog::logger> Logger;
    };

    #define PL_LOG_IMPL(level, category, fmt, ...)  {LogSystem::Log(spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, level, category, fmt, ## __VA_ARGS__);}
    #define PL_LOG_WITHOUT_SOURCE(level, category, fmt, ...)  {LogSystem::Log(level, category, fmt, ## __VA_ARGS__);}

    #define PL_VERBOSE(category, fmt, ...) PL_LOG_WITHOUT_SOURCE(spdlog::level::trace, category, fmt, ## __VA_ARGS__)
    #define PL_INFO(category, fmt, ...) PL_LOG_WITHOUT_SOURCE(spdlog::level::info, category, fmt, ## __VA_ARGS__)
    #define PL_WARN(category, fmt, ...) PL_LOG_IMPL(spdlog::level::warn, category, fmt, ## __VA_ARGS__)
    #define PL_ERROR(category, fmt, ...) PL_LOG_IMPL(spdlog::level::err, category, fmt, ## __VA_ARGS__)
    #define PL_FATAL(category, fmt, ...) PL_LOG_IMPL(spdlog::level::critical, category, fmt, ## __VA_ARGS__)

    #define LOG_INFO(category, fmt, ...) PL_LOG_WITHOUT_SOURCE(spdlog::level::info, category, u##fmt, ## __VA_ARGS__)
    // #define LOG(category, level, fmt, ...) PL_LOG_IMPL(spdlog::level::warn, category, fmt, ## __VA_ARGS__)
    #define CLOG(expr, category, level, fmt, ...) \
        if (expr)                                          \
        {                                                  \
            PL_LOG_IMPL(level, category, fmt, ## __VA_ARGS__) \
        }
#ifdef DEBUG
    #define PL_ASSERT(expr) ((expr) ? (void)0 : LogSystem::AssertFail(spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, #expr))
#else
    #define PL_ASSERT(expr)
#endif
}
