#pragma once

#include "definitions_core.hpp"
#include "global.hpp"
#include "memory/allocator_policies.hpp"
#include "math/generic_math.hpp"
#include "foundation/details/compressed_pair.hpp"

namespace Engine
{
    template <typename ValueType>
    class BitRef
    {
    public:
        BitRef(ValueType& data, ValueType mask)
            : Data(data)
            , Mask(mask)
        {}

        operator bool() const
        {
            return (Data & Mask) != 0;
        }

        BitRef& operator= (bool value)
        {
            ValueType& data = const_cast<ValueType&>(Data);
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
        const ValueType& Data;
        ValueType Mask;
    };

    template <typename ContainerType>
    class ConstBitRef
    {
    public:
        using SizeType = typename ContainerType::SizeType;
        using ValueType = typename ContainerType::ValueType;

        ConstBitRef(const ValueType& data, ValueType mask)
            : Data(data)
            , Mask(mask)
        {}

        operator bool() const
        {
            return (Data & Mask) != 0;
        }

        explicit operator BitRef<ValueType>() const
        {
            return BitRef<ValueType>(const_cast<ValueType&>(Data), Mask);
        }
    private:
        const ValueType& Data;
        ValueType Mask;
    };

    template <typename ContainerType>
    class RelativeBitRef
    {
    public:
        using SizeType = typename ContainerType::SizeType;
        using ValueType = typename ContainerType::ValueType;

        explicit RelativeBitRef(SizeType bitIndex)
            : Index(bitIndex >> ContainerType::ELEMENT_BITS_NUM_LOG_TWO)
            , Mask(1 << (bitIndex & (ContainerType::ELEMENT_BITS_NUM - 1)))
        {}

        SizeType Index;
        ValueType Mask;
    };

    /**
     * A const iterator always move to next 'true' index
     * @tparam ContainerType
     */
    template <typename ContainerType>
    class ConstBitValidIterator : public RelativeBitRef<ContainerType>
    {
        using Super = RelativeBitRef<ContainerType>;
        using ValueType = typename ContainerType::ValueType;
        using SizeType = typename ContainerType::SizeType;
        using USizeType = std::make_unsigned_t<SizeType>;
    public:
        explicit ConstBitValidIterator(const ContainerType& container, SizeType startIndex = 0)
            : Super(startIndex)
            , Container(container)
            , CurrentBitIndex(startIndex)
            , UnvisitedBitMask((~0U) << (startIndex & (ContainerType::ELEMENT_BITS_NUM - 1)))
            , BaseBitIndex(startIndex & ~(ContainerType::ELEMENT_BITS_NUM - 1))
        {
            if (startIndex != Container.Size())
            {
                FindFirstSetBit();
            }
        }

        ConstBitRef<ContainerType> operator* () const
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
            UnvisitedBitMask &= ~this->Mask;

            // Contains the first set bit that hasn't been visited yet.
            FindFirstSetBit();

            return *this;
        }

        SizeType GetIndex() const { return CurrentBitIndex; }

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
            const ValueType* rawData = Container.Data();
            const SizeType arraySize = Container.Size();
            const SizeType lastElemIndex = (arraySize - 1) / ContainerType::ELEMENT_BITS_NUM;

            // Advance to the next non-zero uint32.
            ValueType remainingBitMask = rawData[this->Index] & UnvisitedBitMask;
            while (!remainingBitMask)
            {
                ++this->Index;
                BaseBitIndex += ContainerType::ELEMENT_BITS_NUM;
                if (this->Index > lastElemIndex)
                {
                    // We've advanced past the end of the array.
                    CurrentBitIndex = arraySize;
                    return;
                }

                remainingBitMask = rawData[this->Index];
                UnvisitedBitMask = ~0;
            }

            // This operation has the effect of unsetting the lowest set bit of BitMask
            const ValueType newRemainingBitMask = remainingBitMask & (remainingBitMask - 1);

            // This operation XORs the above mask with the original mask, which has the effect
            // of returning only the bits which differ; specifically, the lowest bit
            this->Mask = newRemainingBitMask ^ remainingBitMask;

            // If the Nth bit was the lowest set bit of BitMask, then this gives us N
            CurrentBitIndex = BaseBitIndex + ContainerType::ELEMENT_BITS_NUM - 1 - Math::CountLeadingZeros(this->Mask);

            // If we've accidentally iterated off the end of an array but still within the same DWORD
            // then set the index to the last index of the array
            if (CurrentBitIndex > arraySize)
            {
                CurrentBitIndex = arraySize;
            }
        }
    protected:
        const ContainerType& Container;
        SizeType CurrentBitIndex;
        USizeType UnvisitedBitMask;
        USizeType BaseBitIndex;
    };

    /**
     * An iterator always move to next 'true' index
     * @tparam ContainerType
     */
    template <typename ContainerType>
    class BitValidIterator : public ConstBitValidIterator<ContainerType>
    {
        using Super = ConstBitValidIterator<ContainerType>;
        using ValueType = typename ContainerType::ValueType;
        using SizeType = typename ContainerType::SizeType;
    public:
        BitValidIterator(ContainerType& container, SizeType startIndex = 0)
            : Super(container, startIndex)
        {}

        BitRef<ValueType> operator* () const
        {
            return static_cast<BitRef<ValueType>>(this->Container[this->CurrentBitIndex]);
        }

        BitValidIterator& operator++()
        {
            Super::operator++();
            return *this;
        }

        SizeType GetIndex() const { return Super::GetIndex(); }

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
        using ValueType = typename ContainerType::ValueType;
        using SizeType = typename ContainerType::SizeType;
    public:
        explicit ConstBitIndexIterator(const ContainerType& container, SizeType index = 0)
            : Container(container)
            , Index(index)
        {}

        ConstBitRef<ContainerType> operator* () const
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

        SizeType GetIndex() const { return Index; }

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
        SizeType Index;
    };

    template <typename ContainerType>
    class BitIndexIterator : public ConstBitIndexIterator<ContainerType>
    {
        using Super = ConstBitIndexIterator<ContainerType>;
        using ValueType = typename ContainerType::ValueType;
        using SizeType = typename ContainerType::SizeType;
    public:
        explicit BitIndexIterator(const ContainerType& container, SizeType index = 0) : Super(container, index)
        {}

        BitRef<ValueType> operator* () const
        {
            return static_cast<BitRef<uint32>>(this->Container[this->Index]);
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

        SizeType GetIndex() const { return this->Index; }

        friend bool operator==(const BitIndexIterator& lhs, const BitIndexIterator& rhs)
        {
            return lhs.Index == rhs.Index && &lhs.Container == &rhs.Container;
        }

        friend bool operator!=(const BitIndexIterator& lhs, const BitIndexIterator& rhs)
        {
            return !(lhs == rhs);
        }
    };

    template<typename ValueType, typename SizeType, typename Pointer>
    class BitArrayVal
    {
    public:
        BitArrayVal() = default;

        ~BitArrayVal() = default;

        Pointer Data{ nullptr };
        SizeType Size{ 0 };
        SizeType Capacity{ 0 };
    };

    template <typename Alloc = StandardAllocator<int32>>
    class BitArray
    {
    public:
        using AllocatorType = typename Alloc::template ElementAllocator<uint32>;
        using ValueType = typename AllocatorType::ValueType;
        using SizeType = typename AllocatorType::SizeType;

        /** A const iterator always move to next 'true' index */
        using ConstValidIterator = ConstBitValidIterator<BitArray>;
        /** An iterator always move to next 'true' index */
        using ValidIterator = BitValidIterator<BitArray>;
        /** A const iterator move to next index */
        using ConstIterator = ConstBitIndexIterator<BitArray>;
        /** An iterator move to next index */
        using Iterator = BitIndexIterator<BitArray>;

    protected:
        using USizeType = std::make_unsigned_t<SizeType>;
        using SecondaryVal = BitArrayVal<ValueType, SizeType, ValueType*>;

    public:
        static constexpr SizeType ELEMENT_BITS_NUM = sizeof(ValueType) * 8;
        static constexpr SizeType ELEMENT_BITS_NUM_LOG_TWO = Math::FloorLogTwo((USizeType)ELEMENT_BITS_NUM);
        static constexpr ValueType ELEMENT_MASK = (ValueType)-1;

        BitArray() = default;

        explicit BitArray(SizeType capacity, const AllocatorType& alloc = AllocatorType())
            : Pair(OneArgPlaceholder(), alloc)
        {
            Reserve(capacity);
        }

        BitArray(bool val, SizeType size, const AllocatorType& alloc = AllocatorType())
            : Pair(OneArgPlaceholder(), alloc)
        {
            Resize(size, val);
        }

        BitArray(const bool* ptr, SizeType size, const AllocatorType& alloc = AllocatorType())
            : Pair(OneArgPlaceholder(), alloc)
        {
            if (size > 0)
            {
                Reserve(size);
                InitBits(ptr, size);
            }
        }

        BitArray(std::initializer_list<bool> initializer, const AllocatorType& alloc = AllocatorType())
            : Pair(OneArgPlaceholder(), alloc)
        {
            SizeType size = static_cast<SizeType>(initializer.size());
            if (size > 0)
            {
                Reserve(size);
                InitBits(initializer.begin(), size);
            }
        }

        BitArray(const BitArray& other)
            : Pair(OneArgPlaceholder(), other.GetAlloc())
        {
            CopyBits(const_cast<ValueType*>(other.Data()), other.Size());
        }

        BitArray(BitArray&& other) noexcept
            : Pair(OneArgPlaceholder(), std::move(other.GetAlloc()))
        {
            MoveArray(std::forward<BitArray>(other));
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

        BitRef<ValueType> operator[] (SizeType index)
        {
            ENSURE(0 <= index && index < Size());
            return BitRef<ValueType>(Data()[index / ELEMENT_BITS_NUM], 1 << (index & (ELEMENT_BITS_NUM - 1)));
        }

        ConstBitRef<BitArray> operator[] (SizeType index) const
        {
            ENSURE(0 <= index && index < Size());
            return ConstBitRef<BitArray>(Data()[index / ELEMENT_BITS_NUM], 1 << (index & (ELEMENT_BITS_NUM - 1)));
        }

        BitArray& operator= (std::initializer_list<bool> initializer)
        {
            Clear(static_cast<SizeType>(initializer.size()));
            InitBits(initializer.begin(), static_cast<SizeType>(initializer.size()));
            return *this;
        }

        BitArray& operator= (const BitArray& other)
        {
            ENSURE(this != &other);
            GetAlloc() = other.GetAlloc();
            CopyBits(const_cast<uint32*>(other.Data()), other.Size());
            return *this;
        }

        BitArray& operator= (BitArray&& other) noexcept
        {
            ENSURE(this != &other);
            GetAlloc() = std::move(other.GetAlloc());
            MoveArray(std::forward<BitArray>(other));
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
            auto& myVal = Pair.SecondVal;
            auto& otherVal = other.Pair.SecondVal;
            if (myVal.Size != otherVal.Size)
            {
                return false;
            }

            if (myVal.Size % ELEMENT_BITS_NUM == 0)
            {
                return Memory::Memcmp(myVal.Data, otherVal.Data, myVal.Size / ELEMENT_BITS_NUM * sizeof(ValueType));
            }

            SizeType elemCount = myVal.Size / ELEMENT_BITS_NUM;
            if (Memory::Memcmp(myVal.Data, otherVal.Data, elemCount * sizeof(ValueType)) == false)
            {
                return false;
            }

            SizeType offset = (myVal.Size - 1) % ELEMENT_BITS_NUM;
            ValueType mask = (~0u << (offset + 1));
            SizeType curIdx = (myVal.Size - 1) / ELEMENT_BITS_NUM;
            const ValueType& myData = myVal.Data[curIdx];
            const ValueType& otherData = otherVal.Data[curIdx];
            return (myData | mask) == (otherData | mask);
        }

        bool operator!= (const BitArray& other) const
        {
            return !(*this == other);
        }

        void Add(bool value)
        {
            SizeType index = AddBits(1);
            ValueType& data = Data()[index / ELEMENT_BITS_NUM];
            SizeType bitOffset = (index % ELEMENT_BITS_NUM);
            data = (data & ~(1 << bitOffset)) | (((ValueType)value) << bitOffset);
        }

        void Insert(SizeType index, bool value)
        {
            ENSURE(0 <= index && index <= Size());
            InsertBits(index, 1);

            // Work out which uint32 index to set from, and how many
            SizeType startIndex = index / ELEMENT_BITS_NUM;
            SizeType count = (index + 1 + (ELEMENT_BITS_NUM - 1)) / ELEMENT_BITS_NUM - startIndex;

            // Work out masks for the start/end of the sequence
            USizeType startMask = ELEMENT_MASK << (index % ELEMENT_BITS_NUM);
            USizeType endMask = ELEMENT_MASK >> (ELEMENT_BITS_NUM - (index + 1) % ELEMENT_BITS_NUM) % ELEMENT_BITS_NUM;

            ValueType* data = Data() + startIndex;
            if (value)
            {
                if (count == 1)
                {
                    *data |= startMask & endMask;
                }
                else
                {
                    *data++ |= startMask;
                    count -= 2;
                    while (count != 0)
                    {
                        *data++ = ~0;
                        --count;
                    }
                    *data |= endMask;
                }
            }
            else
            {
                if (count == 1)
                {
                    *data &= ~(startMask & endMask);
                }
                else
                {
                    *data++ &= ~startMask;
                    count -= 2;
                    while (count != 0)
                    {
                        *data++ = 0;
                        --count;
                    }
                    *data &= ~endMask;
                }
            }
        }

        void RemoveAt(SizeType index, SizeType num = 1)
        {
            auto& myVal = Pair.SecondVal;
            ENSURE(index >= 0 && num >= 0 && index + num <= myVal.Size);

            if (index + num != myVal.Size)
            {
                SizeType numToShift = myVal.Size - (index + num);
                Memory::MemmoveBits(myVal.Data, index, myVal.Data, index + num, numToShift);
            }

            myVal.Size -= num;
        }

        bool Find(bool elem, SizeType& index) const
        {
            index = Find(elem);
            return index != INDEX_NONE;
        }

        SizeType Find(bool elem) const
        {
            auto& myVal = Pair.SecondVal;
            const ValueType test = elem ? 0u : ~0u;

            const ValueType* data = myVal.Data;
            const SizeType size = myVal.Size;
            const SizeType elemCount = Math::DivideAndCeil(size, ELEMENT_BITS_NUM);
            SizeType elemIndex = 0;
            while (elemIndex < elemCount && data[elemIndex] == test)
            {
                ++elemIndex;
            }

            if (elemIndex < elemCount)
            {
                // If we're looking for a false, then we flip the bits - then we only need to find the first one bit
                const ValueType bits = elem ? (data[elemIndex]) : ~(data[elemIndex]);
                ENSURE(bits != 0);
                const SizeType lowestBitIndex = Math::CountTrailingZeros(bits) + (elemIndex << ELEMENT_BITS_NUM_LOG_TWO);
                if (lowestBitIndex < size)
                {
                    return lowestBitIndex;
                }
            }

            return INDEX_NONE;
        }

        bool FindLast(bool elem, SizeType& index) const
        {
            index = FindLast(elem);
            return index != INDEX_NONE;
        }

        SizeType FindLast(bool elem) const
        {
            auto& myVal = Pair.SecondVal;
            SizeType size = myVal.Size;

            // Get the correct mask for the last word
            SizeType slackIndex = ((size - 1) % ELEMENT_BITS_NUM) + 1;
            USizeType mask = ~0u >> (ELEMENT_BITS_NUM - slackIndex);

            // Iterate over the array until we see a word with a zero bit.
            SizeType elemIndex = Math::DivideAndCeil(size, ELEMENT_BITS_NUM);
            const ValueType* data = myVal.Data;
            const ValueType test = elem ? 0u : ~0u;
            while (true)
            {
                if (elemIndex == 0)
                {
                    return INDEX_NONE;
                }
                --elemIndex;
                if ((data[elemIndex] & mask) != (test & mask))
                {
                    break;
                }
                mask = ~0u;
            }

            // Flip the bits, then we only need to find the first one bit -- easy.
            const ValueType bits = (elem ? data[elemIndex] : ~data[elemIndex]) & mask;
            ENSURE(bits != 0);

            SizeType bitIndex = (ELEMENT_BITS_NUM - 1) - Math::CountLeadingZeros(bits);

            SizeType result = bitIndex + (elemIndex << ELEMENT_BITS_NUM_LOG_TWO);
            return result;
        }

        bool Contains(bool elem) const
        {
            return Find(elem) != INDEX_NONE;
        }

        void Resize(SizeType newSize)
        {
            Resize(newSize, false);
        }

        void Resize(SizeType newSize, bool val)
        {
            ENSURE(newSize >= 0);
            auto& myVal = Pair.SecondVal;
            if (newSize > myVal.Size)
            {
                Reserve(newSize);

                SizeType curIdx = myVal.Size <= 0 ? -1 : (myVal.Size - 1) / ELEMENT_BITS_NUM;
                SizeType elemCount = Math::DivideAndCeil(newSize, ELEMENT_BITS_NUM);
                if (myVal.Size > 0)
                {
                    ValueType& data = myVal.Data[curIdx];
                    SizeType bitOffset = ((myVal.Size - 1) % ELEMENT_BITS_NUM);
                    if (val)
                    {
                        data = (data | ~0u << (bitOffset + 1));
                    }
                    else
                    {
                        data = (data & ((1 << bitOffset) - 1));
                    }
                }

                while (++curIdx < elemCount)
                {
                    myVal.Data[curIdx] = val ? ~0u : 0u;
                }
            }

            myVal.Size = newSize;
        }

        void Reserve(SizeType newCapacity)
        {
            auto& myVal = Pair.SecondVal;
            if (newCapacity > myVal.Capacity)
            {
                Reallocate(newCapacity);
            }
        }

        void Clear(SizeType slack = 0)
        {
            if (slack == 0)
            {
                Tidy();
            }
            else
            {
                Pair.SecondVal.Size = 0;
                Reallocate(slack);
            }
        }

        SizeType Size() const { return Pair.SecondVal.Size;  }

        SizeType GetElementCount() const
        {
            return Math::DivideAndCeil(Size(), ELEMENT_BITS_NUM);
        }

        SizeType Capacity() const { return Pair.SecondVal.Capacity; }

        constexpr SizeType MaxSize() const
        {
            return NumericLimits<SizeType>::Max();
        }

        ValueType* Data()
        {
            return Pair.SecondVal.Data;
        }

        const ValueType* Data() const
        {
            return Pair.SecondVal.Data;
        }

        AllocatorType GetAllocator() const
        {
            return static_cast<AllocatorType>(GetAlloc());
        }

        ValidIterator CreateValidIterator(SizeType startIndex = 0)
        {
            ENSURE(startIndex >= 0 && startIndex <= Size());
            return ValidIterator(*this, startIndex);
        }

        ConstValidIterator CreateValidIterator(SizeType startIndex = 0) const
        {
            ENSURE(startIndex >= 0 && startIndex <= Size());
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
            return Iterator(*this, Size());
        }

        ConstIterator end() const
        {
            return ConstIterator(*this, Size());
        }
    private:
        AllocatorType& GetAlloc()
        {
            return Pair.GetFirst();
        }

        const AllocatorType& GetAlloc() const
        {
            return Pair.GetFirst();
        }

        void InitBits(const bool* ptr, SizeType size)
        {
            ENSURE(size > 0);
            for (SizeType idx = 0; idx < size; ++idx)
            {
                Add(ptr[idx]);
            }
        }

        SizeType AddBits(SizeType count)
        {
            ENSURE(count > 0);
            auto& myVal = Pair.SecondVal;
            SizeType index = myVal.Size;
            myVal.Size += count;
            if (myVal.Size > myVal.Capacity)
            {
                Expansion(myVal.Size);
            }
            return index;
        }

        void InsertBits(SizeType index, SizeType count)
        {
            auto& myVal = Pair.SecondVal;
            ENSURE(index >= 0 && index <= myVal.Size);
            SizeType oldCount = myVal.Size;
            myVal.Size += count;
            if (myVal.Size > myVal.Capacity)
            {
                Expansion(myVal.Size);
            }

            SizeType shiftCount = oldCount - index;
            if (shiftCount > 0)
            {
                Memory::MemmoveBits(myVal.Data, index + count, myVal.Data, index, shiftCount);
            }
        }

        void CopyBits(ValueType* data, SizeType size)
        {
            ENSURE(size > 0);
            Clear(size);
            auto& myVal = Pair.SecondVal;
            myVal.Size = size;
            if (size > 0)
            {
                Memory::Memcpy(myVal.Data, data, GetElementCount() * sizeof(ValueType));
            }
        }

        void SetBits(ValueType* dest, bool value, SizeType size)
        {
            const SizeType elemCount = Math::DivideAndCeil(size, ELEMENT_BITS_NUM);
            if (elemCount > 8)
            {
                Memory::Memset(dest, value ? 0xff : 0, elemCount * sizeof(ValueType));
            }
            else
            {
                ValueType elem = value ? ~0u : 0u;
                for (SizeType i = 0; i < elemCount; ++i)
                {
                    dest[i] = elem;
                }
            }
        }

        void MoveArray(BitArray&& other)
        {
            auto& myVal = Pair.SecondVal;
            auto& otherVal = other.Pair.SecondVal;
            if (myVal.Data)
            {
                auto& alloc = GetAlloc();
                alloc.Deallocate(myVal.Data, myVal.Capacity);
            }

            myVal.Size = otherVal.Size;
            myVal.Capacity = otherVal.Capacity;
            myVal.Data = otherVal.Data;
            otherVal.Data = nullptr;
            otherVal.Size = 0;
            otherVal.Capacity = 0;
        }

        void Expansion(SizeType destSize = -1)
        {
            destSize = destSize >= 0 ? destSize : Pair.SecondVal.Size;
            SizeType capacity = CalculateGrowth(destSize);
            ENSURE(destSize <= capacity);
            Reallocate(capacity);
        }

        SizeType CalculateGrowth(const SizeType newSize) const
        {
            SizeType oldCapacity = Capacity();
            SizeType max = MaxSize();

            if (oldCapacity > max - oldCapacity / 2)
            {
                return max;
            }

            const SizeType geometric = oldCapacity + oldCapacity / 2;

            if (geometric < newSize)
            {
                return newSize;
            }

            return geometric;
        }

        void NormalizeOffset(uint32* data, SizeType& offset)
        {
            if ((offset < 0) | (ELEMENT_BITS_NUM <= offset))
            {
                data += (offset / ELEMENT_BITS_NUM);
                offset = offset % ELEMENT_BITS_NUM;
                if (offset < 0)
                {
                    --data;
                    offset += ELEMENT_BITS_NUM;
                }
            }
        }

        void Reallocate(SizeType newCapacity)
        {
            auto& myVal = Pair.SecondVal;
            auto& alloc = Pair.GetFirst();

            SizeType elemCount = Math::DivideAndCeil(newCapacity, ELEMENT_BITS_NUM);
            newCapacity = elemCount * ELEMENT_BITS_NUM;

            ValueType* newPtr = alloc.Allocate(elemCount);
            if (myVal.Data)
            {
                Memory::Memmove(newPtr, myVal.Data, myVal.Size);
                alloc.Deallocate(myVal.Data, myVal.Capacity);
            }

            myVal.Data = newPtr;
            myVal.Capacity = newCapacity;
        }

        void Tidy()
        {
            auto& myVal = Pair.SecondVal;
            if (myVal.Data)
            {
                auto& alloc = GetAlloc();
                alloc.Deallocate(myVal.Data, myVal.Capacity);
            }
            myVal.Size = 0;
            myVal.Capacity = 0;
            myVal.Data = nullptr;
        }

    private:
        CompressedPair<AllocatorType, SecondaryVal> Pair;
    };
}