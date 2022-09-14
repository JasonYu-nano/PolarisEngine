#pragma once

#include "global.hpp"

namespace Engine
{
    enum class ECaseSensitivity : uint8
    {
        Sensitive,
        Insensitive
    };

    enum class ESplitBehavior : uint8
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
