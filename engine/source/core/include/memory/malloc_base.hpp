#pragma once

#include <new>
#include "memory/malloc_interface.hpp"

namespace Engine
{
    class CORE_API MallocBase : public IMalloc
    {
    public:
        virtual ~MallocBase() = default;

        virtual void* Malloc(size_t size, uint32 alignment) const = 0;

        virtual void Free(void* ptr) const = 0;

        virtual void* Realloc(void* ptr, size_t size, uint32 alignment) const = 0;

        /** override global operator new/delete to avoid endless loops */
        void* operator new(size_t size) { return std::malloc(size); }

        void* operator new[](size_t size) { return std::malloc(size); }

        void operator delete(void* ptr) { std::free(ptr); }

        void operator delete[](void* ptr) {std::free(ptr); }
    };
}
