#include "precompiled_core.hpp"
#include "predefine/common_marco.hpp"
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
        const IMalloc* gMalloc = GetGMalloc();
        return gMalloc->Malloc(size, alignment);
    }

    void Memory::Free(void* ptr)
    {
        const IMalloc* gMalloc = GetGMalloc();
        gMalloc->Free(ptr);
    }

    void Memory::Shutdown()
    {
        if (GMalloc != nullptr)
        {
            delete GMalloc;
            GMalloc = nullptr;
        }
    }

    const IMalloc* Memory::GetGMalloc()
    {
        if (GMalloc == nullptr)
        {
            GMalloc = PlatformMemory::GetDefaultMalloc();
        }
        ASSERT(GMalloc);
        return GMalloc;
    }
}