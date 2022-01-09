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

        static void* Realloc(void* ptr, size_t newSize, uint32 alignment = PlatformMemory::GetDefaultAlignment());

        static void Memcpy(void* dest, void* src, size_t size);

        /**
         * Copies the values of num bytes from the location pointed by source to the memory block pointed by destination. 
         * Copying takes place as if an intermediate buffer were used, allowing the destination and source to overlap.
         */
        static void Memmove(void* dest, void* src, size_t size);

        static void Memset(void* dest, uint8 byte, size_t size);

        /**
         * see UE4 FBitArrayMemory::MemmoveBitsWordOrder
         * 
         * @param startDest
         * @param destOffset
         * @param startSrc
         * @param srcOffset
         * @param bitCount
         */
        static void MemmoveBits(uint32* startDest, int32 destOffset, uint32* startSrc, int32 srcOffset, uint32 bitCount);

        /** call on app terminal */
        static void Shutdown();
    private:
        Memory() = delete;

        static void NormalizeOffset(uint32* data, int32& offset);

        /** get global malloc object, thread unsafe */
        static IMalloc* GetGMalloc();
    };
}

CORE_API extern Engine::IMalloc* GMalloc;