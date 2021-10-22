#pragma once

#include "core/memory/malloc_interface.hpp"
#include "core/math/align_utils.hpp"

namespace Engine
{
    class MallocC : public IMalloc
    {
    public:
        virtual void* Malloc(size_t size, uint32 alignment) final
        {
            void* ptr = malloc(size + alignment + sizeof(void*) + sizeof(size_t));
            void* result;
            result = Align((uint8*)ptr + sizeof(void*) + sizeof(size_t), alignment);
            *((void**)((uint8*)result - sizeof(void*))) = ptr;
            *((size_t*)((uint8*)result - sizeof(void*) - sizeof(size_t))) = size;
            return result;
        }

        virtual void Free(void* ptr) final
        {
            free(*((void**)((uint8*)ptr - sizeof(void*))));
        }
    };
}