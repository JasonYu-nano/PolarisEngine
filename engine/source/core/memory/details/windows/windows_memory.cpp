#include "precompiled_core.hpp"
#include "memory/details/windows/windows_memory.hpp"
#include "memory/ansi_c_malloc.hpp"

namespace Engine
{
    uint32 WindowsMemory::SDefaultAlignment = 16;

    IMalloc* WindowsMemory::GetDefaultMalloc()
    {
        return new AnsiCMalloc();
    }

    uint32 WindowsMemory::GetDefaultAlignment()
    {
        return SDefaultAlignment;
    }

    void WindowsMemory::Memcpy(void* dest, void* src, size_t size)
    {
        ::memcpy(dest, src, size);
    }

    void WindowsMemory::Memmove(void* dest, void* src, size_t size)
    {
        ::memmove(dest, src, size);
    }

    void WindowsMemory::Memset(void *dest, uint8 byte, size_t size)
    {
        ::memset(dest, byte, size);
    }

    bool WindowsMemory::Memcmp(void* lBuffer, void* rBuffer, size_t size)
    {
        return ::memcmp(lBuffer, rBuffer, size) == 0;
    }
}