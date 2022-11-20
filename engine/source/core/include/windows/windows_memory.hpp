#pragma once

#include "definitions_core.hpp"
#include "global.hpp"
#include "memory/malloc_interface.hpp"

namespace Engine
{
    class CORE_API WindowsMemory
    {
    public:
        static IMalloc* GetDefaultMalloc();

        static uint32 GetDefaultAlignment();

        static void Memcpy(void* dest, void const* src, size_t size);

        static void Memmove(void* dest, void* src, size_t size);

        static void Memset(void* dest, uint8 byte, size_t size);

        static bool Memcmp(void* lBuffer, void* rBuffer, size_t size);
    private:
        static uint32 SDefaultAlignment;
    };

    typedef WindowsMemory PlatformMemory;
}