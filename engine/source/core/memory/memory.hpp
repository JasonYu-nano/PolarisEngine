#pragma once

#include "memory/platform_memory.hpp"

namespace Engine
{
    class CORE_API Memory
    {
    public:
        NODISCARD static void* Malloc(size_t size);

        NODISCARD static void* Malloc(size_t size, uint32 alignment);

        static void Free(void* ptr);

        static void Memcpy(void* dest, void* src, size_t size);

        /** call on app terminal */
        static void Shutdown();
    private:
        Memory() = delete;

        /** get global malloc object, thread unsafe */
        static IMalloc* GetGMalloc();
    };
}

CORE_API extern Engine::IMalloc* GMalloc;