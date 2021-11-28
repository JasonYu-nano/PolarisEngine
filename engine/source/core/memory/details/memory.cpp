#include "precompiled_core.hpp"
#include "core_minimal_private.hpp"
#include "memory/memory.hpp"
#include "memory/malloc_interface.hpp"
#include "math/limit.hpp"

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

    void* Memory::Realloc(void* ptr, size_t newSize, uint32 alignment)
    {
        IMalloc* gMalloc = GetGMalloc();
        return gMalloc->Realloc(ptr, newSize, alignment);
    }

    void Memory::Memcpy(void* dest, void* src, size_t size)
    {
        PlatformMemory::Memcpy(dest, src, size);
    }

    void Memory::Memmove(void* dest, void* src, size_t size)
    {
        PlatformMemory::Memmove(dest, src, size);
    }

    void Memory::MemmoveBits(uint32* startDest, int32 destOffset, uint32* startSrc, int32 srcOffset, uint32 bitCount)
    {
        if (bitCount <= 0)
        {
            return;
        }

        NormalizeOffset(startDest, destOffset);
        NormalizeOffset(startSrc, srcOffset);

        const uint32 uintBits = 32;

        // If the Dest words are aligned with the Source Words, use the aligned version of MemmoveBits which requires fewer masking instructions.
        // This also allows us to reduce complexity by not needing to handle the aligned case in the rest of this function.
        const int32 DeltaOffset = destOffset - srcOffset;
        if (DeltaOffset == 0)
        {
            //MemmoveBitsWordOrderAlignedInternal(startDest, startSrc, destOffset, bitCount);
            const int32 endOffset = (destOffset + bitCount - 1) % uintBits + 1;
            const uint32 startMask = MAX_UINT32 << destOffset;
            const uint32 endMask = MAX_UINT32 >> (uintBits - endOffset);

            // Calculate Start and Final Pointers
            const uint32 offsetToLastWord = (destOffset + bitCount - 1) / MAX_UINT32;
            uint32* const finalDest = startDest + offsetToLastWord;
            const uint32* const finalSource = startSrc + offsetToLastWord;

            // If the end word is the start word, use special handling to apply both the start mask and the end mask
            if (offsetToLastWord == 0)
            {
                uint32 combinedMask = startMask & endMask;
                *startDest = (*startDest & ~combinedMask) | (*startSrc & combinedMask);
                return;
            }

            // If the Dest range and Source range overlap, we need to dest the words of dest in the same order as the direction from deststart to sourcestart, so that words are read from the destrange before being overwritten
            // If Dest is less than Source, this is handled by forward iteration, which is the default since it is better for cache coherency
            // If Source is less than Dest AND they overlap, we need to iterate backwards
            const bool bRequiresBackward = (startSrc < startDest) & (startDest <= finalSource);
            if (!bRequiresBackward)
            {
                *startDest = (*startDest & ~startMask) | (*startSrc & startMask);
                uint32* currentDest = startDest + 1;
                const uint32* CurrentSource = startSrc + 1;
                while (currentDest < finalDest)
                {
                    *currentDest++ = *CurrentSource++;
                }
                *finalDest = (*finalSource & endMask) | (*finalDest & ~endMask);
            }
            else
            {
                *finalDest = (*finalSource & endMask) | (*finalDest & ~endMask);
                uint32* currentDest = finalDest - 1;
                const uint32* currentSource = finalSource - 1;
                while (currentDest > startDest)
                {
                    *currentDest-- = *currentSource--;
                }
                *startDest = (*startDest & ~startMask) | (*startSrc & startMask);
            }
            return;
        }

        // Work out shifts for which bits of each source word are in each of the two dest words it overlaps 
        const uint32 firstBitInLowerDestWord = (DeltaOffset >= 0) ? DeltaOffset : DeltaOffset + uintBits;
        const uint32 upShiftToLowerDestWord = firstBitInLowerDestWord; // The shift up distance to align with the dest positions of the lower dest word is equal to the position of the first bit in that word. We copy it to this variable name to make it easier to understand why we're using it as the shift
        const uint32 downShiftToUpperDestWord = uintBits - firstBitInLowerDestWord; // The shift down distance to align with the upper dest word is the additive inverse of the shift-up distance

        // Calculate Starting and Ending DestMasks
        const uint32 endDestOffset = (destOffset + bitCount - 1) % uintBits + 1;
        const uint32 startDestMask = destOffset == 0 ? MAX_UINT32 : (MAX_UINT32 << destOffset);
        const uint32 endDestMask = MAX_UINT32 >> (uintBits - endDestOffset);

        // Calculate Final Pointers
        uint32* const finalDest = startDest + (destOffset + bitCount - 1) / uintBits;
        const uint32* const finalSource = startSrc + (srcOffset + bitCount - 1) / uintBits;

        // If the end word is the start word, use special handling to apply both the start mask and the end mask
        if (startDest == finalDest)
        {
            uint32 combinedDestMask = startDestMask & endDestMask;
            // Read the bits from the lower and upper source word of the dest word, if they are in the source range
            // Note we are not allowed to try reading from any word that does not overlap the source range because it could pointing to inaccessible memory.
            if (srcOffset < destOffset)
            {
                // The beginning of the first source word is past the beginning of the dest word, so it is the upper source word of the dest word
                // It must therefore also be the last source word.
                // Shift it up to the offset of the dest word.
                const uint32 destBitsFromUpperSourceWord = *(startSrc) << upShiftToLowerDestWord;
                *startDest = (*startDest & ~combinedDestMask) | (destBitsFromUpperSourceWord & combinedDestMask);
            }
            else if (startSrc == finalSource)
            {
                // The beginning of the first source word is before the beginning of the dest word, so it is the lower source word of the dest word
                // But the write range ends at or before the end of the source word, so we only have a single source word
                // Shift it down to the offset of the dest word
                const uint32 destBitsFromLowerSourceWord = ((*startSrc) >> downShiftToUpperDestWord);
                *startDest = (*startDest & ~combinedDestMask) | (destBitsFromLowerSourceWord & combinedDestMask);
            }
            else
            {
                // The beginning of the first source word is before the beginning of the dest word, so it is the lower source word of the dest word
                // The write range ends after the end of the source word, so we also have an upper source word - finalSource is the upper source word of the destword
                // Shift startSrc down and finalSource up
                const uint32 destBitsFromLowerSourceWord = ((*startSrc) >> downShiftToUpperDestWord);
                const uint32 destBitsFromUpperSourceWord = *(finalSource) << upShiftToLowerDestWord;
                *startDest = (*startDest & ~combinedDestMask) | ((destBitsFromLowerSourceWord | destBitsFromUpperSourceWord) & combinedDestMask);
            }
            return;
        }

        // If the Dest range and Source range overlap, we need to dest the words of dest in the same order as the direction from deststart to sourcestart, so that words are read from the destrange before being overwritten
        // If Dest is less than Source, this is handled by forward enumeration, which is the default since it is better for cache coherency
        // If Source is less than Dest AND they overlap, we need to enumerate backwards
        const bool bRequiresBackward = (startSrc < startDest) & (startDest <= finalSource);
        if (!bRequiresBackward)
        {
            uint32* currentDest = startDest;
            const uint32* currentSource = startSrc;
            uint32 destBitsFromLowerSourceWord;
            uint32 destBitsFromUpperSourceWord;
            uint32 nextDestBitsFromLowerSourceWord;

            // Write the first Dest word, handling whether the first source word is the lower source word or the upper source word and handling startDestMask
            if (srcOffset < destOffset)
            {
                // The beginning of the first source word is past the beginning of the dest word, so it is the upper source word of the dest word, and we don't have bits from the lower source word
                // Note we are not allowed to try reading from any word that does not overlap the source range because it could pointing to inaccessible memory, so we don't try to read the bits from currentSource-1
                destBitsFromUpperSourceWord = *(currentSource) << upShiftToLowerDestWord;
                nextDestBitsFromLowerSourceWord = *(currentSource) >> downShiftToUpperDestWord; // Must read this before writing currentDest to handle overlap
                *currentDest = (*currentDest & ~(startDestMask)) | (destBitsFromUpperSourceWord & startDestMask);
            }
            else
            {
                // The first source word is the lower source word of the first dest word
                destBitsFromLowerSourceWord = *(currentSource) >> downShiftToUpperDestWord;
                ++currentSource;
                destBitsFromUpperSourceWord = *(currentSource) << upShiftToLowerDestWord;
                nextDestBitsFromLowerSourceWord = *(currentSource) >> downShiftToUpperDestWord; // Must read this before writing currentDest to handle overlap
                *currentDest = (*currentDest & ~(startDestMask)) | ((destBitsFromLowerSourceWord | destBitsFromUpperSourceWord) & startDestMask);
            }

            // Establish the loop invariant: we have written all dest before currentDest, currentSource is the lower source word of currentDest, and destBitsFromLowerSourceWord has been read
            // Note that reading all bits from currentSource before writing any dest words it touches is necessary if the source range and dest range overlap and the source words are within one word of their dest words
            ++currentDest;
            // currentSource remains unchanged; it is the lower source word of the new currentDest
            destBitsFromLowerSourceWord = nextDestBitsFromLowerSourceWord;

            while (currentDest != finalDest)
            {
                ++currentSource;
                destBitsFromUpperSourceWord = *(currentSource) << upShiftToLowerDestWord;
                nextDestBitsFromLowerSourceWord = *(currentSource) >> downShiftToUpperDestWord; // Must read this before writing currentDest to handle overlap
                *currentDest = destBitsFromLowerSourceWord | destBitsFromUpperSourceWord;
                ++currentDest;
                destBitsFromLowerSourceWord = nextDestBitsFromLowerSourceWord;
            }

            // Write the final Dest word, handling whether the last source word is the lower source word or the upper source word and handling endmask
            if (endDestOffset <= firstBitInLowerDestWord)
            {
                // The last dest word ends before the bits touched by its upper source word, so the current lower source word is the last source word
                // Note we are not allowed to try reading from any word that does not overlap the source range because it could pointing to inaccessible memory, so we don't try to read the bits from currentSource+1
                destBitsFromUpperSourceWord = 0;
            }
            else
            {
                // The last source word is the upper source word of the last dest word; increment one last time and read the UpperSourceWord bits
                ++currentSource;
                destBitsFromUpperSourceWord = *(currentSource) << upShiftToLowerDestWord;
            }
            *currentDest = ((destBitsFromLowerSourceWord | destBitsFromUpperSourceWord) & endDestMask) | (*currentDest & ~(endDestMask));
        }
        else
        {
            uint32* currentDest = finalDest;
            const uint32* currentSource = finalSource;
            uint32 destBitsFromLowerSourceWord;
            uint32 destBitsFromUpperSourceWord;
            uint32 nextDestBitsFromUpperSourceWord;

            // Write the final Dest word, handling whether the last source word is the lower source word or the upper source word and handling endmask
            if (endDestOffset <= firstBitInLowerDestWord)
            {
                // The last dest word ends before the bits touched by its upper source word, so the final source word is the lower source word for the final dest word, and we don't have any bits from the upper source word
                // Note we are not allowed to try reading from any word that does not overlap the source range because it could pointing to inaccessible memory, so we don't try to read the bits from currentSource+1
                destBitsFromLowerSourceWord = *(currentSource) >> downShiftToUpperDestWord;
                nextDestBitsFromUpperSourceWord = *(currentSource) << upShiftToLowerDestWord; // Must read this before writing currentDest to handle overlap
                *currentDest = (destBitsFromLowerSourceWord & endDestMask) | (*currentDest & ~(endDestMask));
            }
            else
            {
                // The last source word is the upper source word of the last dest word
                destBitsFromUpperSourceWord = *(currentSource) << upShiftToLowerDestWord;
                --currentSource;
                destBitsFromLowerSourceWord = *(currentSource) >> downShiftToUpperDestWord;
                nextDestBitsFromUpperSourceWord = *(currentSource) << upShiftToLowerDestWord; // Must read this before writing currentDest to handle overlap
                *currentDest = ((destBitsFromLowerSourceWord | destBitsFromUpperSourceWord) & endDestMask) | (*currentDest & ~(endDestMask));
            }

            // Establish the loop invariant: we have written all dest before currentDest, currentSource is the lower source word of currentDest, and destBitsFromLowerSourceWord has been read
            // Note that reading all bits from currentSource before writing any dest words it touches is necessary if the source range and dest range overlap and the source words are within one word of their dest words
            --currentDest;
            // currentSource remains unchanged; it is the upper source word of the new currentDest
            destBitsFromUpperSourceWord = nextDestBitsFromUpperSourceWord;

            while (currentDest != startDest)
            {
                --currentSource;
                destBitsFromLowerSourceWord = *(currentSource) >> downShiftToUpperDestWord;
                nextDestBitsFromUpperSourceWord = *(currentSource) << upShiftToLowerDestWord; // Must read this before writing currentDest to handle overlap
                *currentDest = destBitsFromLowerSourceWord | destBitsFromUpperSourceWord;
                --currentDest;
                destBitsFromUpperSourceWord = nextDestBitsFromUpperSourceWord;
            }

            // Write the first Dest word, handling whether the first source word is the lower source word or the upper source word and handling startDestMask
            if (srcOffset < destOffset)
            {
                // The beginning of the first source word is past the beginning of the dest word, so it is the upper source word of the dest word, and we've already read its bits above
                // Note we are not allowed to try reading from any word that does not overlap the source range because it could pointing to inaccessible memory, so we don't try to read the bits from currentSource-1
                destBitsFromLowerSourceWord = 0;
            }
            else
            {
                // The first source word is the lower source word of the first dest word; decrement one last time and read the LowerSourceWord bits
                --currentSource;
                destBitsFromLowerSourceWord = *(currentSource) >> downShiftToUpperDestWord;
            }
            *currentDest = (*currentDest & ~(startDestMask)) | ((destBitsFromLowerSourceWord | destBitsFromUpperSourceWord) & startDestMask);
        }
    }

    void Memory::Shutdown()
    {
        if (GMalloc != nullptr)
        {
            delete GMalloc;
            GMalloc = nullptr;
        }
    }

    void Memory::NormalizeOffset(uint32* data, int32& offset)
    {
        const uint32 uintBits = 32;
        if ((offset < 0) | (uintBits <= offset))
        {
            data += (offset / uintBits);
            offset = offset % uintBits;
            if (offset < 0)
            {
                --data;
                offset += uintBits;
            }
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