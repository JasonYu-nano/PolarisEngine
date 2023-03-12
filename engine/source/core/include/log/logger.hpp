#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/pattern_formatter.h"
#include "foundation/string.hpp"
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
        Off = SPDLOG_LEVEL_OFF,
    };

    class CORE_API GLogCategory
    {

    };

#define DECLARE_LOG_CATEGORY(name) \
    class GLogCategory_##name : public GLogCategory \
    { \
    public: \
        static const String& GetCategoryName() \
        { \
            static String category = #name; \
            return category; \
        } \
    }

    DECLARE_LOG_CATEGORY(LogTemp);

    class CORE_API LogSystem
    {
    public:
        template<typename... Args>
        static void Log(spdlog::source_loc src, ELogLevel level, const String& category, const char* fmt, Args&&... args)
        {
            spdlog::level::level_enum logLevel = spdlog::level::off;
            switch (level)
            {
                case Trace:
                {
                    logLevel = spdlog::level::trace;
                    break;
                }
                case Debug:
                {
                    logLevel = spdlog::level::debug;
                    break;
                }
                case Info:
                {
                    logLevel = spdlog::level::info;
                    break;
                }
                case Warn:
                {
                    logLevel = spdlog::level::warn;
                    break;
                }
                case Error:
                {
                    logLevel = spdlog::level::err;
                    break;
                }
                case Critical:
                {
                    logLevel = spdlog::level::critical;
                    break;
                }
                default:;
            }

            String logText = String::Format(fmt, std::forward<Args>(args)...).Prepend(String::Format("[{0}] ", category));
            GetLogger()->log(src, logLevel, logText);
        }

        template<typename... Args>
        static void Log(ELogLevel level, const String& category, const char* fmt, Args&&... args)
        {
            spdlog::level::level_enum logLevel = spdlog::level::off;
            switch (level)
            {
                case Trace:
                {
                    logLevel = spdlog::level::trace;
                    break;
                }
                case Debug:
                {
                    logLevel = spdlog::level::debug;
                    break;
                }
                case Info:
                {
                    logLevel = spdlog::level::info;
                    break;
                }
                case Warn:
                {
                    logLevel = spdlog::level::warn;
                    break;
                }
                case Error:
                {
                    logLevel = spdlog::level::err;
                    break;
                }
                case Critical:
                {
                    logLevel = spdlog::level::critical;
                    break;
                }
                default:;
            }

            String logText = String::Format(fmt, std::forward<Args>(args)...).Prepend(String::Format("[{0}] ", category));
            GetLogger()->log(logLevel, logText);
        }

        static void AssertFail(spdlog::source_loc src, const char* expr)
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
            auto colorSink = MakeShared<spdlog::sinks::stdout_color_sink_mt>();
            colorSink->set_color(spdlog::level::info, 0xffff);
            sinks.push_back(colorSink);
            sinks.push_back(MakeShared<spdlog::sinks::basic_file_sink_mt>("engine_log", "logs/engine_log.txt"));
            Logger = MakeShared<spdlog::logger>("engine_log", begin(sinks), end(sinks));
            Logger->set_pattern("[%D] [%H:%M:%S] [%l] %v%$");
        }

        SharedPtr<spdlog::logger> Logger;
    };

    #define PL_LOG_IMPL(level, category, fmt, ...)  {LogSystem::Log(spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, level, GLogCategory_##category::GetCategoryName(), fmt, ## __VA_ARGS__);}
    #define PL_LOG_WITHOUT_SOURCE(level, category, fmt, ...)  {LogSystem::Log(level, GLogCategory_##category::GetCategoryName(), fmt, ## __VA_ARGS__);}

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
