#pragma once
#include "definitions_core.hpp"
#include "predefine/platform.hpp"

namespace Engine
{
    class CORE_API IMalloc
    {
    public:
        virtual ~IMalloc() = default;

        virtual void* Malloc(size_t size, uint32 alignment) = 0;

        virtual void Free(void* ptr) = 0;

        virtual void* Realloc(void* ptr, size_t size, uint32 alignment) = 0;
    };
}