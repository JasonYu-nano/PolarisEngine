#pragma once

#include "global.hpp"

namespace Engine
{
    enum ECaseSensitivity : uint8
    {
        CaseSensitive,
        CaseInsensitive
    };

    enum ESplitBehavior : uint8
    {
        KeepEmptyParts,
        SkipEmptyParts,
    };

#if PLATFORM_WINDOWS
    #define PLATFORM_U16_WCHAR 1
#else
    #define PLATFORM_U16_WCHAR 0
#endif
}
