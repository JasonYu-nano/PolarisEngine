#pragma once

#include "memory/malloc_base.hpp"

namespace Engine
{
    class CORE_API AnsiCMalloc : public MallocBase
    {
    public:
        virtual ~AnsiCMalloc() = default;

        virtual void* Malloc(size_t size, uint32 alignment) const final;

        virtual void Free(void* ptr) const final;

        virtual void* Realloc(void* ptr, size_t size, uint32 alignment) const final;
    };
}