#pragma once

#include "global.hpp"
#include "definitions_core.hpp"

namespace Engine
{
    struct CORE_API FileTime
    {
        size_t CreationTime = 0;
        size_t LastAccessTime = 0;
        size_t LastModifyTime = 0;
    };
}
