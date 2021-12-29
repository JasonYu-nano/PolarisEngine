#pragma once
#include "definitions_core.hpp"
#include "predefine/platform.hpp"
#include "memory/heap_allocator.hpp"
#include "math/generic_math.hpp"

namespace Engine
{
    constexpr int32 kElementBits = (int32)32;
    constexpr int32 kElementBitsLogTwo = (int32)5;
    constexpr uint32 kFullWordMask = (uint32)-1;

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

    class ConstBitRef
    {
    public:

        ConstBitRef(const uint32& data, uint32 mask)
            : Data(data)
            , Mask(mask)
        {}

        operator bool() const
        {
            return (Data & Mask) != 0;
        }

    private:
        const uint32& Data;
        uint32 Mask;
    };

    class RelativeBitRef
    {
    public:
         explicit RelativeBitRef(int32 BitIndex)
            : DWORDIndex(BitIndex >> kElementBitsLogTwo)
            , Mask(1 << (BitIndex & (kElementBits - 1)))
        {
        }

        int32 DWORDIndex;
        uint32 Mask;
    };

    template <typename ContainerType>
    class ConstBitIterator : public RelativeBitRef
    {
    public:
        explicit ConstBitIterator(const ContainerType& container, int32 startIndex = 0)
            : RelativeBitRef(startIndex)
            , Container(container)
            , UnvisitedBitMask((~0U) << (startIndex & (kElementBits - 1)))
            , CurrentBitIndex(startIndex)
            , BaseBitIndex(startIndex & ~(kElementBits - 1))
        {
            if (startIndex != Container.GetCount())
            {
                FindFirstSetBit();
            }
        }

        ConstBitIterator& operator++()
        {
            // Mark the current bit as visited.
            UnvisitedBitMask &= ~Mask;

            // Contains the first set bit that hasn't been visited yet.
            FindFirstSetBit();

            return *this;
        }

        int32 GetIndex() const { return CurrentBitIndex; }

        friend bool operator==(const ConstBitIterator& lhs, const ConstBitIterator& rhs)
        {
            return lhs.CurrentBitIndex == rhs.CurrentBitIndex && &lhs.Container == &rhs.Container;
        }

        friend bool operator!=(const ConstBitIterator& lhs, const ConstBitIterator& rhs)
        {
            return !(lhs == rhs);
        }

    private:
        void FindFirstSetBit()
        {
            const uint32* rawData = Container.GetData();
            const int32 arrayCount = Container.GetCount();
            const int32 lastDWORDIndex = (arrayCount - 1) / kElementBits;

            // Advance to the next non-zero uint32.
            uint32 remainingBitMask = rawData[DWORDIndex] & UnvisitedBitMask;
            while (!remainingBitMask)
            {
                ++DWORDIndex;
                BaseBitIndex += kElementBits;
                if (DWORDIndex > lastDWORDIndex)
                {
                    // We've advanced past the end of the array.
                    CurrentBitIndex = arrayCount;
                    return;
                }

                remainingBitMask = rawData[DWORDIndex];
                UnvisitedBitMask = ~0;
            }

            // This operation has the effect of unsetting the lowest set bit of BitMask
            const uint32 newRemainingBitMask = remainingBitMask & (remainingBitMask - 1);

            // This operation XORs the above mask with the original mask, which has the effect
            // of returning only the bits which differ; specifically, the lowest bit
            Mask = newRemainingBitMask ^ remainingBitMask;

            // If the Nth bit was the lowest set bit of BitMask, then this gives us N
            CurrentBitIndex = BaseBitIndex + kElementBits - 1 - Math::CountLeadingZeros(Mask);

            // If we've accidentally iterated off the end of an array but still within the same DWORD
            // then set the index to the last index of the array
            if (CurrentBitIndex > arrayCount)
            {
                CurrentBitIndex = arrayCount;
            }
        }
    protected:
        const ContainerType& Container;
        uint32 UnvisitedBitMask;
        int32 CurrentBitIndex;
        uint32 BaseBitIndex;
    };

    template <typename ContainerType>
    class BitIterator : public ConstBitIterator<ContainerType>
    {
        using Super = ConstBitIterator<ContainerType>;
    public:
        BitIterator(ContainerType& container, uint32 startIndex = 0)
            : Super(container, startIndex)
        {}

        BitIterator& operator++()
        {
            Super::operator++();

            return *this;
        }

        int32 GetIndex() const { return Super::GetIndex(); }

        friend bool operator==(const BitIterator& lhs, const BitIterator& rhs)
        {
            return lhs.CurrentBitIndex == rhs.CurrentBitIndex && &lhs.Container == &rhs.Container;
        }

        friend bool operator!=(const BitIterator& lhs, const BitIterator& rhs)
        {
            return !(lhs == rhs);
        }
    };

    template <typename Allocator = HeapAllocator<int32>>
    class BitArray
    {
    public:
        using ConstIterator = ConstBitIterator<BitArray>;
        using Iterator = BitIterator<BitArray>;
    public:
        BitArray() 
            : Capacity(static_cast<int32>(AllocatorInstance.GetDefaultCapacity()) * kElementBits)
        {
            AllocatorInstance.Resize(Math::DivideAndCeil(Capacity, kElementBits), kElementBits);
        };

        explicit BitArray(int32 capacity)
            : Capacity(Math::Max(capacity, static_cast<int32>(AllocatorInstance.GetDefaultCapacity()) * kElementBits))
        {
            AllocatorInstance.Resize(Math::DivideAndCeil(Capacity, kElementBits), kElementBits);
        }

        //TODO: impl
        BitArray(bool defaultValue, int32 count);

        BitRef operator[] (int32 index)
        {
            PL_ASSERT(0 <= index && index < Count);
            return BitRef(GetData()[index / kElementBits], 1 << (index & (kElementBits - 1)));
        }

        ConstBitRef operator[] (int32 index) const
        {
            PL_ASSERT(0 <= index && index < Count);
            return ConstBitRef(GetData()[index / kElementBits], 1 << (index & (kElementBits - 1)));
        }

        void Add(bool value)
        {
            int32 index = AddUnconstructElement(1);
            uint32& data = GetData()[index / kElementBits];
            uint32 bitOffset = (index % kElementBits);
            data = (data & ~(1 << bitOffset)) | (((uint32)value) << bitOffset);
        }

        void Insert(int32 index, bool value)
        {
            PL_ASSERT(0 <= index && index <= GetCount())
            InsertUnconstructElement(index, 1);
            uint32& data = GetData()[index / kElementBits];

            // Work out which uint32 index to set from, and how many
            int32 startIndex = index / kElementBits;
            int32 count = (index + 1 + (kElementBits - 1)) / kElementBits - startIndex;

            // Work out masks for the start/end of the sequence
            uint32 StartMask = kFullWordMask << (index % kElementBits);
            uint32 EndMask = kFullWordMask >> (kElementBits - (index + 1) % kElementBits) % kElementBits;

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

        int32 GetCount() const { return Count;  }

        int32 GetCapacity() const { return Capacity; }

        uint32* GetData()
        {
            return (uint32*)AllocatorInstance.GetAllocation();
        }

        const uint32* GetData() const
        {
            return (const uint32*)AllocatorInstance.GetAllocation();
        }
    private:
        int32 AddUnconstructElement(int32 count)
        {
            PL_ASSERT(count > 0);
            int32 oldCount = Count;
            Count += count;
            if (Count > Capacity)
            {
                Expansion();
            }
            return oldCount;
        }

        void InsertUnconstructElement(int32 index, int32 count)
        {
            PL_ASSERT(index >= 0 && index <= Count);
            int32 oldCount = Count;
            Count += count;
            if (Count > Capacity)
            {
                Expansion();
            }

            int32 shiftCount = oldCount - index;
            if (shiftCount > 0)
            {
                Memory::MemmoveBits(GetData(), index + count, GetData(), index, shiftCount);
            }
        }

        void Expansion()
        {
            int32 elementCount = AllocatorInstance.CaculateValidCapacity(Math::DivideAndCeil(Count, kElementBits), Capacity / kElementBits, kElementBits);
            AllocatorInstance.Resize(elementCount, kElementBits);
            Capacity = elementCount * kElementBits;
            PL_ASSERT(Count <= Capacity);
        }

        void NormalizeOffset(uint32* data, int32& offset)
        {
            if ((offset < 0) | (kElementBits <= offset))
            {
                data += (offset / kElementBits);
                offset = offset % kElementBits;
                if (offset < 0)
                {
                    --data;
                    offset += kElementBits;
                }
            }
        }

    private:
        Allocator AllocatorInstance;
        int32 Count{ 0 }; //as bit
        int32 Capacity{ 0 }; //as bit
    };
}