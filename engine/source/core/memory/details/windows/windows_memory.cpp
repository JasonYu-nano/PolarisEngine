#include "precompiled_core.hpp"
#include "memory/details/windows/windows_memory.hpp"
#include "core/memory/malloc_c.hpp"

namespace Engine
{
    IMalloc* WindowsMemory::SDefaultMalloc = new MallocC();
    uint32 WindowsMemory::SDefaultAlignment = 16;

    const IMalloc* WindowsMemory::GetDefaultMalloc()
    {
        return SDefaultMalloc;
    }

    uint32 WindowsMemory::GetDefaultAlignment()
    {
        return 1;
    }
}