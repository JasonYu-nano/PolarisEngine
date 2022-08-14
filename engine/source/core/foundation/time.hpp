#pragma once

#include <chrono>
#include "definitions_core.hpp"

namespace Engine
{
    using system_clock = std::chrono::system_clock;

    template <typename clock, typename duration = typename clock::duration>
    using time_point = std::chrono::time_point<clock, duration>;

    using TimestampDuration = std::chrono::duration<int64, std::milli>;

    struct PlatformClock;

    class CORE_API Timestamp
    {
    public:
        using Duration = TimestampDuration;

        Timestamp() = default;

        explicit Timestamp(TimestampDuration duration)
            : Dur(duration)
        {}

        NODISCARD time_point<PlatformClock, Duration> ToTimePoint() const
        {
            return time_point<PlatformClock, Duration>(Dur);
        }

        NODISCARD int64 TimeSinceEpoch() const
        {
            return Dur.count() * Duration::period::num / Duration::period::den;
        }

    private:
        Duration Dur;
    };

    struct PlatformClock
    {
        using rep = TimestampDuration::rep;

        using period = TimestampDuration::period;

        using duration                  = TimestampDuration;
        using time_point                = std::chrono::time_point<PlatformClock>;
        static constexpr bool is_steady = false;

        NODISCARD static Timestamp Now() noexcept
        {
            return Timestamp(std::chrono::duration_cast<duration, system_clock::rep, system_clock::period>(
                    system_clock::now().time_since_epoch()));
        }
    };
}
