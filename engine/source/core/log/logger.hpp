#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/pattern_formatter.h"
#include "foundation/ustring.hpp"
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
        template<typename... Args>
        static void Log(spdlog::source_loc src, ELogLevel level, const char* category, const char* fmt, Args&&... args)
        {
            switch (level)
            {
                case Trace:
                {
                    GetLogger()->log(src, spdlog::level::trace, UString::Format(fmt, Forward<Args>(args)...));
                    break;
                }
                case Debug:
                {
                    GetLogger()->log(src, spdlog::level::debug, UString::Format(fmt, Forward<Args>(args)...));
                    break;
                }
                case Info:
                {
                    GetLogger()->log(src, spdlog::level::info, UString::Format(fmt, Forward<Args>(args)...));
                    break;
                }
                case Warn:
                {
                    GetLogger()->log(src, spdlog::level::warn, UString::Format(fmt, Forward<Args>(args)...));
                    break;
                }
                case Error:
                {
                    GetLogger()->log(src, spdlog::level::err, UString::Format(fmt, Forward<Args>(args)...));
                    break;
                }
                case Critical:
                {
                    GetLogger()->log(src, spdlog::level::critical, UString::Format(fmt, Forward<Args>(args)...));
                    break;
                }
                default:;
            }
        }

        template<typename... Args>
        static void Log(ELogLevel level, const char* category, const char* fmt, Args&&... args)
        {
            switch (level)
            {
                case Trace:
                {
                    GetLogger()->log(spdlog::level::trace, UString::Format(fmt, Forward<Args>(args)...));
                    break;
                }
                case Debug:
                {
                    GetLogger()->log(spdlog::level::debug, UString::Format(fmt, Forward<Args>(args)...));
                    break;
                }
                case Info:
                {
                    GetLogger()->log(spdlog::level::info, UString::Format(fmt, Forward<Args>(args)...));
                    break;
                }
                case Warn:
                {
                    GetLogger()->log(spdlog::level::warn, UString::Format(fmt, Forward<Args>(args)...));
                    break;
                }
                case Error:
                {
                    GetLogger()->log(spdlog::level::err, UString::Format(fmt, Forward<Args>(args)...));
                    break;
                }
                case Critical:
                {
                    GetLogger()->log(spdlog::level::critical, UString::Format(fmt, Forward<Args>(args)...));
                    break;
                }
                default:;
            }
        }

        template<typename... Args>
        static void Log(spdlog::level::level_enum level, const ansi* category, const char* fmt, Args&&... args)
        {
            GetLogger()->log(level, UString::Format(fmt, Forward<Args>(args)...));
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

    #define LOG_VERBOSE(category, fmt, ...) PL_LOG_WITHOUT_SOURCE(Trace, category, fmt, ## __VA_ARGS__)
    #define LOG_INFO(category, fmt, ...) PL_LOG_WITHOUT_SOURCE(Info, category, fmt, ## __VA_ARGS__)
    #define LOG_WARN(category, fmt, ...) PL_LOG_IMPL(Warn, category, fmt, ## __VA_ARGS__)
    #define LOG_ERROR(category, fmt, ...) PL_LOG_IMPL(Error, category, fmt, ## __VA_ARGS__)
    #define LOG_FATAL(category, fmt, ...) PL_LOG_IMPL(Critical, category, fmt, ## __VA_ARGS__)

    #define CLOG(expr, category, level, fmt, ...) \
        if (expr)                                          \
        {                                                  \
            PL_LOG_IMPL(level, category, fmt, ## __VA_ARGS__) \
        }
}
