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
}
