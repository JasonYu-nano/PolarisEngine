#pragma once
#include "definitions_core.hpp"
#include "global.hpp"
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
            if (startIndex != Container.Size())
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
            return CurrentBitIndex < Container.Size();
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
            const uint32* rawData = Container.Data();
            const int32 arrayCount = Container.Size();
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

    template <typename Allocator = DefaultAllocator>
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

        using AllocatorType = DefaultAllocator::ElementAllocator<int32>;
    public:
        BitArray() 
            : ArrayCapacity(AllocatorInstance.GetDefaultSize() * kElementBits)
        {
            AllocatorInstance.Resize(Math::DivideAndCeil(ArrayCapacity, kElementBits));
        };

        explicit BitArray(int32 capacity)
            : ArrayCapacity(Math::Max(Math::CeilToMultiple(capacity, kElementBits),
                 AllocatorInstance.GetDefaultSize() * kElementBits))
        {
            AllocatorInstance.Resize(Math::DivideAndCeil(ArrayCapacity, kElementBits));
        }

        BitArray(bool defaultValue, int32 count)
        {
            AddBits(count);
            SetBits(Data(), defaultValue, GetElementCount());
        }

        BitArray(const bool* ptr, int32 count)
        {
            ENSURE(count >= 0);
            Resize(Math::CeilToMultiple(count, kElementBits));
            InitBits(ptr, count);
        }

        BitArray(std::initializer_list<bool> initializer)
        {
            int32 count = static_cast<int32>(initializer.size());
            ENSURE(count >= 0);
            Resize(Math::CeilToMultiple(count, kElementBits));
            InitBits(initializer.begin(), count);
        }

        BitArray(const BitArray& other)
        {
            CopyBits(const_cast<uint32*>(other.Data()), other.Size());
        }

        BitArray(BitArray&& other) noexcept
        {
            MoveArray(Forward<BitArray>(other));
        }

        template <typename OtherAllocator>
        explicit BitArray(const BitArray<OtherAllocator>& other)
        {
            CopyBits(other.Data(), other.Size());
        }

        ~BitArray()
        {
            Clear(0);
        }

        BitRef operator[] (int32 index)
        {
            ENSURE(0 <= index && index < ArraySize);
            return BitRef(Data()[index / kElementBits], 1 << (index & (kElementBits - 1)));
        }

        ConstBitRef operator[] (int32 index) const
        {
            ENSURE(0 <= index && index < ArraySize);
            return ConstBitRef(Data()[index / kElementBits], 1 << (index & (kElementBits - 1)));
        }

        BitArray& operator= (std::initializer_list<bool> initializer)
        {
            Clear(static_cast<int32>(initializer.size()));
            InitBits(initializer.begin(), static_cast<int32>(initializer.size()));
            return *this;
        }

        BitArray& operator= (const BitArray& other)
        {
            ENSURE(this != &other);
            CopyBits(const_cast<uint32*>(other.Data()), other.Size());
            return *this;
        }

        BitArray& operator= (BitArray&& other) noexcept
        {
            ENSURE(this != &other);
            MoveArray(Forward<BitArray>(other));
            return *this;
        }

        template <typename OtherAllocator>
        BitArray& operator= (const BitArray<OtherAllocator>& other)
        {
            CopyBits(other.Data(), other.Size());
            return *this;
        }

        bool operator== (const BitArray& other) const
        {
            if (ArraySize != other.ArraySize)
            {
                return false;
            }

            return Memory::Memcmp((void*) Data(), (void*) other.Data(), GetElementCount() * sizeof(uint32));
        }

        void Add(bool value)
        {
            int32 index = AddBits(1);
            uint32& data = Data()[index / kElementBits];
            uint32 bitOffset = (index % kElementBits);
            data = (data & ~(1 << bitOffset)) | (((uint32)value) << bitOffset);
        }

        void Insert(int32 index, bool value)
        {
            ENSURE(0 <= index && index <= Size());
            InsertUnconstructElement(index, 1);

            // Work out which uint32 index to set from, and how many
            int32 startIndex = index / kElementBits;
            int32 count = (index + 1 + (kElementBits - 1)) / kElementBits - startIndex;

            // Work out masks for the start/end of the sequence
            uint32 StartMask = kFullWordMask << (index % kElementBits);
            uint32 EndMask = kFullWordMask >> (kElementBits - (index + 1) % kElementBits) % kElementBits;

            uint32* data = Data() + startIndex;
            if (value)
            {
                if (count == 1)
                {
                    *data |= StartMask & EndMask;
                }
                else
                {
                    *data++ |= StartMask;
                    count -= 2;
                    while (count != 0)
                    {
                        *data++ = ~0;
                        --count;
                    }
                    *data |= EndMask;
                }
            }
            else
            {
                if (count == 1)
                {
                    *data &= ~(StartMask & EndMask);
                }
                else
                {
                    *data++ &= ~StartMask;
                    count -= 2;
                    while (count != 0)
                    {
                        *data++ = 0;
                        --count;
                    }
                    *data &= ~EndMask;
                }
            }
        }

        void Resize(int32 capacity)
        {
            ENSURE(capacity >= 0);
            const int32 defaultBitCount = AllocatorInstance.GetDefaultSize() * kElementBits;
            const int32 newCapacity = Math::Max(defaultBitCount, Math::CeilToMultiple(capacity, kElementBits));
            if (newCapacity != ArrayCapacity)
            {
                AllocatorInstance.Resize(Math::DivideAndCeil(newCapacity, kElementBits));
                ArrayCapacity = newCapacity;
            }
        }

        void Clear(int32 slack = 0)
        {
            ArraySize = 0;
            Resize(slack);
        }

        int32 Size() const { return ArraySize;  }

        int32 GetElementCount() const
        {
            return Math::DivideAndCeil(ArraySize, kElementBits);
        }

        int32 Capacity() const { return ArrayCapacity; }

        constexpr int32 MaxSize() const
        {
            return MAX_INT32;
        }

        uint32* Data()
        {
            return (uint32*)AllocatorInstance.GetAllocation();
        }

        const uint32* Data() const
        {
            return (const uint32*)AllocatorInstance.GetAllocation();
        }

        ValidIterator CreateValidIterator(int32 startIndex = 0)
        {
            ENSURE(startIndex >= 0 && startIndex <= ArraySize);
            return ValidIterator(*this, startIndex);
        }

        ConstValidIterator CreateValidIterator(int32 startIndex = 0) const
        {
            ENSURE(startIndex >= 0 && startIndex <= ArraySize);
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
            return Iterator(*this, ArraySize);
        }

        ConstIterator end() const
        {
            return ConstIterator(*this, ArraySize);
        }
    private:
        void InitBits(const bool* ptr, int32 count)
        {
            ENSURE(count > 0);
            for (int idx = 0; idx < count; ++idx)
            {
                Add(ptr[idx]);
            }
        }

        int32 AddBits(int32 count)
        {
            ENSURE(count > 0);
            int32 oldCount = ArraySize;
            ArraySize += count;
            if (ArraySize > ArrayCapacity)
            {
                Expansion();
            }
            return oldCount;
        }

        void InsertUnconstructElement(int32 index, int32 count)
        {
            ENSURE(index >= 0 && index <= ArraySize);
            int32 oldCount = ArraySize;
            ArraySize += count;
            if (ArraySize > ArrayCapacity)
            {
                Expansion();
            }

            int32 shiftCount = oldCount - index;
            if (shiftCount > 0)
            {
                Memory::MemmoveBits(Data(), index + count, Data(), index, shiftCount);
            }
        }

        void CopyBits(uint32* data, int32 count)
        {
            ENSURE(count > 0);
            Clear(count);
            ArraySize = count;
            if (count > 0)
            {
                Memory::Memcpy(Data(), data, GetElementCount() * sizeof(uint32));
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
            ArraySize = other.ArraySize;
            ArrayCapacity = other.ArrayCapacity;
            other.ArraySize = 0;
            other.ArrayCapacity = 0;
        }

        void Expansion()
        {
            int32 capacity = CalculateGrowth(ArraySize);
            int32 elementCount = Math::DivideAndCeil(ArraySize, kElementBits);
            AllocatorInstance.Resize(elementCount);
            ArrayCapacity = elementCount * kElementBits;
            ENSURE(ArraySize <= ArrayCapacity);
        }

        int32 CalculateGrowth(const int32 newSize) const
        {
            int32 oldCapacity = Capacity();
            int32 max = MaxSize();

            if (oldCapacity > max - oldCapacity / 2)
            {
                return max;
            }

            const int32 geometric = oldCapacity + oldCapacity / 2;

            if (geometric < newSize)
            {
                return newSize;
            }

            return geometric;
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
        AllocatorType AllocatorInstance;
        int32 ArraySize{ 0 }; //as bit
        int32 ArrayCapacity{ 0 }; //as bit, always is a multiple of kElementBits
    };
}