#pragma once

#include "global.hpp"
#include "foundation/time.hpp"
#include "windows/minimal_windows.hpp"

namespace Engine
{
    constexpr uint64 k_UnixTimeStart = 0x019DB1DED53E8000;

    Timestamp FileTimeToTimestamp(const FILETIME& time)
    {
        ULARGE_INTEGER li;
        li.LowPart = time.dwLowDateTime;
        li.HighPart = time.dwHighDateTime;
        uint64 unixTime = (li.QuadPart - k_UnixTimeStart) / 10'000'000;


        return Timestamp(Timestamp::Duration(unixTime));
    }
}
