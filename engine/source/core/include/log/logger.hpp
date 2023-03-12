#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/pattern_formatter.h"
#include "foundation/string.hpp"
#include "foundation/smart_ptr.hpp"
#include "file_system/path.hpp"
#include "file_system/file_system.hpp"

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
        static spdlog::logger* GetLogger() \
        { \
            static UniquePtr<spdlog::logger> logger; \
            if (logger == nullptr) \
            { \
                std::vector<spdlog::sink_ptr> sinks; \
                auto colorSink = MakeShared<spdlog::sinks::stdout_color_sink_mt>(); \
                colorSink->set_color(spdlog::level::info, 0xffff); \
                sinks.push_back(colorSink); \
                String save = Path::Combine(FileSystem::GetEngineSaveDir(), "logs/engine_log.txt"); \
                sinks.push_back(MakeShared<spdlog::sinks::basic_file_sink_mt>(save.Data(), true)); \
                logger = MakeUnique<spdlog::logger>(#name, begin(sinks), end(sinks)); \
                logger->set_pattern("[%n] [%D] [%H:%M:%S] [%l] %v%$"); \
            } \
            return logger.get(); \
        } \
    }

    DECLARE_LOG_CATEGORY(LogTemp);

    class CORE_API LogSystem
    {
    public:
        template<typename... Args>
        static void Log(spdlog::logger* logger, spdlog::source_loc src, ELogLevel level, fmt::format_string<Args...> fmt, Args&&... args)
        {
            logger->log(src, CastLevel(level), fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Log(spdlog::logger* logger, ELogLevel level, fmt::format_string<Args...> fmt, Args&&... args)
        {
            logger->log(CastLevel(level), fmt, std::forward<Args>(args)...);
        }

    private:
        static constexpr spdlog::level::level_enum CastLevel(ELogLevel level)
        {
            switch (level)
            {
                case Trace:
                    return spdlog::level::trace;
                case Debug:
                    return spdlog::level::debug;
                case Info:
                    return spdlog::level::info;
                case Warn:
                    return spdlog::level::warn;
                case Error:
                    return spdlog::level::err;
                case Critical:
                    return spdlog::level::critical;
                default:
                    return spdlog::level::off;
            }
        }
    };

    #define PL_LOG_IMPL(level, category, fmt, ...)  {LogSystem::Log(GLogCategory_##category::GetLogger(), spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, level, fmt, ## __VA_ARGS__);}
    #define PL_LOG_WITHOUT_SOURCE(level, category, fmt, ...)  {LogSystem::Log(GLogCategory_##category::GetLogger(), level, fmt, ## __VA_ARGS__);}

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
