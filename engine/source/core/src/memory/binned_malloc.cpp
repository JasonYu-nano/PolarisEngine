//#include "precompiled_core.hpp"
#include "memory/binned_malloc.hpp"

namespace Engine
{
    void* BinnedMalloc::Malloc(size_t size, uint32 alignment)
    {
        return nullptr;
    }

    void BinnedMalloc::Free(void* ptr)
    {
    }

    void* BinnedMalloc::Realloc(void* ptr, size_t size, uint32 alignment)
    {
        return nullptr;
    }

    void BinnedMalloc::SetupCurrentThreadTLS()
    {
    }
}