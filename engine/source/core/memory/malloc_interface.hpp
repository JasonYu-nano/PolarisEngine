#pragma once
#include "core/predefine/platform.hpp"

namespace Engine
{
    class IMalloc
    {
    public:
        virtual ~IMalloc() = default;

        virtual void* Malloc(uint64 size, uint32 alignment) = 0;

        virtual void Free(void* ptr) = 0;
    };
}