#pragma once
#include "definitions_core.hpp"
#include "predefine/platform.hpp"
#include "memory/heap_allocator.hpp"
#include "math/generic_math.hpp"

namespace Engine
{
    class BitRef
    {
    public:
        BitRef(uint32& data, uint32 mask)
            : Data(data)
            , Mask(mask)
        {}

        operator bool() const
        {
            return (Data & Mask) != 0;
        }

        BitRef& operator= (bool value)
        {
            if (value)
            {
                Data |= Mask;
            }
            else
            {
                Data &= ~Mask;
            }
            return* this;
        }
    private:
        uint32& Data;
        uint32 Mask;
    };

    template <typename Allocator = HeapAllocator<uint32>>
    class BitArray
    {
        static constexpr uint32 KElementBits = (uint32)32;
        static constexpr uint32 KFullWordMask = (uint32)-1;

    public:
        BitArray() 
            : Capacity(AllocatorInstance.GetDefaultCapacity() * KElementBits)
        {
            AllocatorInstance.Resize(Math::DivideAndCeil(Capacity, KElementBits), KElementBits);
        };

        BitArray(uint32 capacity)
            : Capacity(Math::Max(capacity, AllocatorInstance.GetDefaultCapacity() * KElementBits))
        {
            AllocatorInstance.Resize(Math::DivideAndCeil(Capacity, KElementBits), KElementBits);
        }

        BitArray(bool defaultValue, uint32 count);

        BitRef operator[] (uint32 index)
        {
            PL_ASSERT(index <= Count);
            return BitRef(GetData()[index / KElementBits], 1 << (index & (KElementBits - 1)));
        }

        const BitRef operator[] (uint32 index) const
        {
            PL_ASSERT(index <= Count);
            return BitRef(GetData()[index / KElementBits], 1 << (index & (KElementBits - 1)));
        }

        void Add(bool value)
        {
            uint32 index = AddUnconstructElement(1);
            uint32& data = GetData()[index / KElementBits];
            uint32 bitOffset = (index % KElementBits);
            data = (data & ~(1 << bitOffset)) | (((uint32)value) << bitOffset);
        }

        void Insert(uint32 index, bool value)
        {
            InsertUnconstructElement(index, 1);
            uint32& data = GetData()[index / KElementBits];

            // Work out which uint32 index to set from, and how many
            uint32 startIndex = index / KElementBits;
            uint32 count = (index + 1 + (KElementBits - 1)) / KElementBits - startIndex;

            // Work out masks for the start/end of the sequence
            uint32 StartMask = KFullWordMask << (index % KElementBits);
            uint32 EndMask = KFullWordMask >> (KElementBits - (index + 1) % KElementBits) % KElementBits;

            uint32* Data = GetData() + startIndex;
            if (value)
            {
                if (count == 1)
                {
                    *Data |= StartMask & EndMask;
                }
                else
                {
                    *Data++ |= StartMask;
                    count -= 2;
                    while (count != 0)
                    {
                        *Data++ = ~0;
                        --count;
                    }
                    *Data |= EndMask;
                }
            }
            else
            {
                if (count == 1)
                {
                    *Data &= ~(StartMask & EndMask);
                }
                else
                {
                    *Data++ &= ~StartMask;
                    count -= 2;
                    while (count != 0)
                    {
                        *Data++ = 0;
                        --count;
                    }
                    *Data &= ~EndMask;
                }
            }
        }

        uint32 GetCount() const { return Count;  }

        uint32 GetCapacity() const { return Capacity; }
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

        void InsertUnconstructElement(uint32 index, uint32 count)
        {
            PL_ASSERT(index >= 0 && index <= Count);
            uint32 oldCount = Count;
            Count += count;
            if (Count > Capacity)
            {
                Expansion();
            }

            uint32 shiftCount = oldCount - index;
            if (shiftCount > 0)
            {
                Memory::MemmoveBits(GetData(), index + count, GetData(), index, shiftCount);
            }
        }

        void Expansion()
        {
            uint32 elementCount = AllocatorInstance.CaculateValidCapacity(Math::DivideAndCeil(Count, KElementBits), Capacity / KElementBits, KElementBits);
            AllocatorInstance.Resize(elementCount, KElementBits);
            Capacity = elementCount * KElementBits;
            PL_ASSERT(Count <= Capacity);
        }

        uint32* GetData()
        {
            return (uint32*)AllocatorInstance.GetAllocation();
        }

        void NormalizeOffset(uint32* data, int32& offset)
        {
            if ((offset < 0) | (KElementBits <= offset))
            {
                data += (offset / KElementBits);
                offset = offset % KElementBits;
                if (offset < 0)
                {
                    --data;
                    offset += KElementBits;
                }
            }
        }

    private:
        Allocator AllocatorInstance;
        uint32 Count{ 0 }; //as bit
        uint32 Capacity{ 0 }; //as bit
    };
}