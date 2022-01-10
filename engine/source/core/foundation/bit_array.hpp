#pragma once
#include "definitions_core.hpp"
#include "predefine/platform.hpp"
#include "memory/allocator_policies.hpp"
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
            uint32& data = const_cast<uint32&>(Data);
            if (value)
            {
                data |= Mask;
            }
            else
            {
                data &= ~Mask;
            }
            return* this;
        }
    private:
        const uint32& Data;
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

        explicit operator BitRef() const
        {
            return BitRef(const_cast<uint32&>(Data), Mask);
        }
    private:
        const uint32& Data;
        uint32 Mask;
    };

#pragma region iterator
    class RelativeBitRef
    {
    public:
         explicit RelativeBitRef(int32 BitIndex)
            : Index(BitIndex >> kElementBitsLogTwo)
            , Mask(1 << (BitIndex & (kElementBits - 1)))
        {}

        int32 Index;
        uint32 Mask;
    };

    /**
     * A const iterator always move to next 'true' index
     * @tparam ContainerType
     */
    template <typename ContainerType>
    class ConstBitValidIterator : public RelativeBitRef
    {
    public:
        explicit ConstBitValidIterator(const ContainerType& container, int32 startIndex = 0)
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

        ConstBitRef operator* () const
        {
            return Container[CurrentBitIndex];
        }

        explicit operator bool() const
        {
            return CurrentBitIndex < Container.GetCount();
        }

        ConstBitValidIterator& operator++()
        {
            // Mark the current bit as visited.
            UnvisitedBitMask &= ~Mask;

            // Contains the first set bit that hasn't been visited yet.
            FindFirstSetBit();

            return *this;
        }

        int32 GetIndex() const { return CurrentBitIndex; }

        friend bool operator==(const ConstBitValidIterator& lhs, const ConstBitValidIterator& rhs)
        {
            return lhs.CurrentBitIndex == rhs.CurrentBitIndex && &lhs.Container == &rhs.Container;
        }

        friend bool operator!=(const ConstBitValidIterator& lhs, const ConstBitValidIterator& rhs)
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
            uint32 remainingBitMask = rawData[Index] & UnvisitedBitMask;
            while (!remainingBitMask)
            {
                ++Index;
                BaseBitIndex += kElementBits;
                if (Index > lastDWORDIndex)
                {
                    // We've advanced past the end of the array.
                    CurrentBitIndex = arrayCount;
                    return;
                }

                remainingBitMask = rawData[Index];
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

    /**
     * An iterator always move to next 'true' index
     * @tparam ContainerType
     */
    template <typename ContainerType>
    class BitValidIterator : public ConstBitValidIterator<ContainerType>
    {
        using Super = ConstBitValidIterator<ContainerType>;
    public:
        BitValidIterator(ContainerType& container, uint32 startIndex = 0)
            : Super(container, startIndex)
        {}

        BitRef operator* () const
        {
            return static_cast<BitRef>(this->Container[this->CurrentBitIndex]);
        }

        BitValidIterator& operator++()
        {
            Super::operator++();
            return *this;
        }

        int32 GetIndex() const { return Super::GetIndex(); }

        friend bool operator==(const BitValidIterator& lhs, const BitValidIterator& rhs)
        {
            return lhs.CurrentBitIndex == rhs.CurrentBitIndex && &lhs.Container == &rhs.Container;
        }

        friend bool operator!=(const BitValidIterator& lhs, const BitValidIterator& rhs)
        {
            return !(lhs == rhs);
        }
    };

    template <typename ContainerType>
    class ConstBitIndexIterator
    {
    public:
        explicit ConstBitIndexIterator(const ContainerType& container, int32 index = 0)
            : Container(container)
            , Index(index)
        {}

        ConstBitRef operator* () const
        {
            return Container[Index];
        }

        ConstBitIndexIterator& operator++ ()
        {
            ++Index;
            return *this;
        }

        ConstBitIndexIterator& operator-- ()
        {
            --Index;
            return *this;
        }

        int32 GetIndex() const { return Index; }

        friend bool operator==(const ConstBitIndexIterator& lhs, const ConstBitIndexIterator& rhs)
        {
            return lhs.Index == rhs.Index && &lhs.Container == &rhs.Container;
        }

        friend bool operator!=(const ConstBitIndexIterator& lhs, const ConstBitIndexIterator& rhs)
        {
            return !(lhs == rhs);
        }
    protected:
        const ContainerType& Container;
        uint32 Index;
    };

    template <typename ContainerType>
    class BitIndexIterator : public ConstBitIndexIterator<ContainerType>
    {
        using Super = ConstBitIndexIterator<ContainerType>;
    public:
        explicit BitIndexIterator(const ContainerType& container, int32 index = 0) : Super(container, index)
        {}

        BitRef operator* () const
        {
            return static_cast<BitRef>(this->Container[this->Index]);
        }

        BitIndexIterator& operator++ ()
        {
            ++this->Index;
            return *this;
        }

        BitIndexIterator& operator-- ()
        {
            --this->Index;
            return *this;
        }

        int32 GetIndex() const { return this->Index; }

        friend bool operator==(const BitIndexIterator& lhs, const BitIndexIterator& rhs)
        {
            return lhs.Index == rhs.Index && &lhs.Container == &rhs.Container;
        }

        friend bool operator!=(const BitIndexIterator& lhs, const BitIndexIterator& rhs)
        {
            return !(lhs == rhs);
        }
    };
#pragma endregion iterator

    template <typename Allocator = HeapAllocator<int32>>
    class BitArray
    {
    public:
        /** A const iterator always move to next 'true' index */
        using ConstValidIterator = ConstBitValidIterator<BitArray>;
        /** An iterator always move to next 'true' index */
        using ValidIterator = BitValidIterator<BitArray>;
        /** A const iterator move to next index */
        using ConstIterator = ConstBitIndexIterator<BitArray>;
        /** An iterator move to next index */
        using Iterator = BitIndexIterator<BitArray>;
    public:
        BitArray() 
            : Capacity(static_cast<int32>(AllocatorInstance.GetDefaultCapacity()) * kElementBits)
        {
            AllocatorInstance.Resize(AllocatorInstance.GetDefaultCapacity(), sizeof(uint32));
        };

        explicit BitArray(int32 capacity)
            : Capacity(Math::Max(Math::CeilToMultiple(capacity, kElementBits),
                 static_cast<int32>(AllocatorInstance.GetDefaultCapacity()) * kElementBits))
        {
            AllocatorInstance.Resize(Math::DivideAndCeil(Capacity, kElementBits), sizeof(uint32));
        }

        BitArray(bool defaultValue, int32 count)
        {
            AddBits(count);
            SetBits(GetData(), defaultValue, GetElementCount());
        }

        BitArray(const bool* ptr, int32 count)
        {
            PL_ASSERT(count >= 0);
            Resize(Math::CeilToMultiple(count, kElementBits));
            InitBits(ptr, count);
        }

        BitArray(std::initializer_list<bool> initializer)
        {
            int32 count = static_cast<int32>(initializer.size());
            PL_ASSERT(count >= 0);
            Resize(Math::CeilToMultiple(count, kElementBits));
            InitBits(initializer.begin(), count);
        }

        BitArray(const BitArray& other)
        {
            CopyBits(const_cast<uint32*>(other.GetData()), other.GetCount());
        }

        BitArray(BitArray&& other) noexcept
        {
            MoveArray(Forward<BitArray>(other));
        }

        template <typename OtherAllocator>
        explicit BitArray(const BitArray<OtherAllocator>& other)
        {
            CopyBits(other.GetData(), other.GetCount());
        }

        ~BitArray()
        {
            Clear(0);
        }

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

        BitArray& operator= (std::initializer_list<bool> initializer)
        {
            Clear(static_cast<int32>(initializer.size()));
            InitBits(initializer.begin(), static_cast<int32>(initializer.size()));
            return *this;
        }

        BitArray& operator= (const BitArray& other)
        {
            PL_ASSERT(this != &other);
            CopyBits(const_cast<uint32*>(other.GetData()), other.GetCount());
            return *this;
        }

        BitArray& operator= (BitArray&& other) noexcept
        {
            PL_ASSERT(this != &other);
            MoveArray(Forward<BitArray>(other));
            return *this;
        }

        template <typename OtherAllocator>
        BitArray& operator= (const BitArray<OtherAllocator>& other)
        {
            CopyBits(other.GetData(), other.GetCount());
            return *this;
        }

        bool operator== (const BitArray& other) const
        {
            if (Count != other.Count)
            {
                return false;
            }

            return Memory::Memcmp((void*)GetData(), (void*)other.GetData(), GetElementCount() * sizeof(uint32));
        }

        void Add(bool value)
        {
            int32 index = AddBits(1);
            uint32& data = GetData()[index / kElementBits];
            uint32 bitOffset = (index % kElementBits);
            data = (data & ~(1 << bitOffset)) | (((uint32)value) << bitOffset);
        }

        void Insert(int32 index, bool value)
        {
            PL_ASSERT(0 <= index && index <= GetCount());
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

        void Resize(int32 capacity)
        {
            PL_ASSERT(capacity >= 0);
            const int32 defaultBitCount = static_cast<int32>(AllocatorInstance.GetDefaultCapacity()) * kElementBits;
            const int32 newCapacity = Math::Max(defaultBitCount, Math::CeilToMultiple(capacity, kElementBits));
            if (newCapacity != Capacity)
            {
                AllocatorInstance.Resize(Math::DivideAndCeil(newCapacity, kElementBits), sizeof(uint32));
                Capacity = newCapacity;
            }
        }

        void Clear(int32 slack = 0)
        {
            Count = 0;
            Resize(slack);
        }

        int32 GetCount() const { return Count;  }

        int32 GetElementCount() const
        {
            return Math::DivideAndCeil(Count, kElementBits);
        }

        int32 GetCapacity() const { return Capacity; }

        uint32* GetData()
        {
            return (uint32*)AllocatorInstance.GetAllocation();
        }

        const uint32* GetData() const
        {
            return (const uint32*)AllocatorInstance.GetAllocation();
        }

        ValidIterator CreateValidIterator(int32 startIndex = 0)
        {
            PL_ASSERT(startIndex >= 0 && startIndex <= Count);
            return ValidIterator(*this, startIndex);
        }

        ConstValidIterator CreateValidIterator(int32 startIndex = 0) const
        {
            PL_ASSERT(startIndex >= 0 && startIndex <= Count);
            return ConstValidIterator(*this, 0);
        }

        Iterator begin()
        {
            return Iterator(*this, 0);
        }

        ConstIterator begin() const
        {
            return ConstIterator(*this, 0);
        }

        Iterator end()
        {
            return Iterator(*this, Count);
        }

        ConstIterator end() const
        {
            return ConstIterator(*this, Count);
        }
    private:
        void InitBits(const bool* ptr, int32 count)
        {
            PL_ASSERT(count > 0);
            for (int idx = 0; idx < count; ++idx)
            {
                Add(ptr[idx]);
            }
        }

        int32 AddBits(int32 count)
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

        void CopyBits(uint32* data, int32 count)
        {
            PL_ASSERT(count > 0);
            Clear(count);
            Count = count;
            if (count > 0)
            {
                Memory::Memcpy(GetData(), data, GetElementCount() * sizeof(uint32));
            }
        }

        void SetBits(uint32* dest, bool value, int32 count)
        {
            const int32 elementCount = Math::DivideAndCeil(count, kElementBits);
            if (elementCount > 8)
            {
                Memory::Memset(dest, value ? 0xff : 0, elementCount * sizeof(int32));
            }
            else
            {
                uint32 element = value ? ~0u : 0u;
                for (int32 i = 0; i < elementCount; ++i)
                {
                    dest[i] = element;
                }
            }
        }

        void MoveArray(BitArray&& other)
        {
            AllocatorInstance = MoveTemp(other.AllocatorInstance);
            Count = other.Count;
            Capacity = other.Capacity;
            other.Count = 0;
            other.Capacity = 0;
        }

        void Expansion()
        {
            int32 elementCount = AllocatorInstance.CalculateValidCapacity(Math::DivideAndCeil(Count, kElementBits),
                                                                          Capacity / kElementBits, sizeof(uint32));
            AllocatorInstance.Resize(elementCount, sizeof(uint32));
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
        int32 Capacity{ 0 }; //as bit, always is a multiple of kElementBits
    };
}