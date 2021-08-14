#pragma once
#include "predefine/platform.hpp"

namespace Engine
{
    struct CORE_API FileTime
    {
        size_t CreationTime = 0;
        size_t LastAccessTime = 0;
        size_t LastModifyTime = 0;
    };
}
