#include "precompiled_core.hpp"
#include "core_minimal_private.hpp"
#include "memory/memory.hpp"
#include "memory/malloc_interface.hpp"

namespace Engine
{
    IMalloc* GMalloc = nullptr;

    void* Memory::Malloc(size_t size)
    {
        return Malloc(size, PlatformMemory::GetDefaultAlignment());
    }

    void* Memory::Malloc(size_t size, uint32 alignment)
    {
        IMalloc* gMalloc = GetGMalloc();
        return gMalloc->Malloc(size, alignment);
    }

    void Memory::Free(void* ptr)
    {
        IMalloc* gMalloc = GetGMalloc();
        gMalloc->Free(ptr);
    }

    void Memory::Memcpy(void* dest, void* src, size_t size)
    {
    }

    void Memory::Shutdown()
    {
        if (GMalloc != nullptr)
        {
            delete GMalloc;
            GMalloc = nullptr;
        }
    }

    IMalloc* Memory::GetGMalloc()
    {
        if (GMalloc == nullptr)
        {
            GMalloc = PlatformMemory::GetDefaultMalloc();
        }
        PL_ASSERT(GMalloc);
        return GMalloc;
    }
}