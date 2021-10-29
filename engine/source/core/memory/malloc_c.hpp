#pragma once

#include "memory/malloc_interface.hpp"

namespace Engine
{
    class CORE_API MallocC : public IMalloc
    {
    public:
        virtual ~MallocC() = default;

        virtual void* Malloc(size_t size, uint32 alignment) final;

        virtual void Free(void* ptr) final;

        virtual void* Realloc(void* ptr, size_t size, uint32 alignment) final;
    };
}