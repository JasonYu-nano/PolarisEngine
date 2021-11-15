#pragma once
#include "definitions_core.hpp"
#include "predefine/platform.hpp"
#include "memory/heap_allocator.hpp"
#include "math/generic_math.hpp"

namespace Engine
{
    template <typename Allocator = HeapAllocator<uint32>>
    class BitArray
    {
    #define ELEMENT_BITS  ((uint32)32)

    public:
        BitArray();

        BitArray(uint32 capacity);

        BitArray(bool defaultValue, uint32 count);

        bool& operator[] (uint32 index);

        void Add(bool value)
        {
            uint32 index = AddUnconstructElement();
            uint32& data = GetData()[index / ELEMENT_BITS];
            uint32 bitOffset = (index % ELEMENT_BITS);
            data = (data & ~(1 << bitOffset)) | (((uint32)value) << bitOffset);
        }
    private:
        uint32 AddUnconstructElement(uint32 count)
        {
            PL_ASSERT(count > 0);
            uint32 oldCount = Count;
            Count += count;
            if (Count > Capacity)
            {
                Expansion();
            }
            return oldCount;
        }

        void Expansion()
        {
            uint32 elementCount = AllocatorInstance.CaculateValidCapacity(Math::DivideAndRoundNearest(Count, ELEMENT_BITS), Capacity / ELEMENT_BITS, ELEMENT_BITS);
            AllocatorInstance.Resize(elementCount, ELEMENT_BITS);
            Capacity = elementCount * ELEMENT_BITS;
            PL_ASSERT(Count <= Capacity);
        }

        uint32* GetData()
        {
            return (uint32*)AllocatorInstance.GetAllocation();
        }

    private:
        Allocator AllocatorInstance;
        uint32 Count{ 0 }; //as bit
        uint32 Capacity{ 0 }; //as bit
    };
}