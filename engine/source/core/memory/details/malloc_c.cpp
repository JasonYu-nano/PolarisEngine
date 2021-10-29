#include "precompiled_core.hpp"
#include "memory/malloc_c.hpp"
#include "math/align_utils.hpp"
#include "math/generic_math.hpp"

namespace Engine
{
    void* MallocC::Malloc(size_t size, uint32 alignment)
    {
        void* ptr = malloc(size + alignment + sizeof(void*) + sizeof(size_t));
        void* result;
        result = Align((uint8*)ptr + sizeof(void*) + sizeof(size_t), alignment);
        *((void**)((uint8*)result - sizeof(void*))) = ptr;
        *((size_t*)((uint8*)result - sizeof(void*) - sizeof(size_t))) = size;
        return result;
    }

    void MallocC::Free(void* ptr)
    {
        free(*((void**)((uint8*)ptr - sizeof(void*))));
    }

    void* MallocC::Realloc(void* ptr, size_t size, uint32 alignment)
    {
        alignment = Math::Max(size >= 16 ? (uint32)16 : (uint32)8, alignment);
        return nullptr;
    }
}