#pragma once

#include "foundation/details/compressed_pair.hpp"
#include "foundation/char_traits.hpp"
#include "memory/allocator_policies.hpp"

namespace Engine
{
    template <typename TChar>
    struct StringVal
    {
        StringVal() {};

        ~StringVal() {};

        using CharType = TChar;
        static constexpr int32 INLINE_SIZE = (16 / sizeof(CharType) < 1) ? 1 : (16 / sizeof(CharType));

        strsize Size{ 0 };
        strsize MaxSize{ INLINE_SIZE };

        union
        {
            CharType* Ptr;
            CharType InlineBuffer[INLINE_SIZE];
        };
    };

    template <typename TChar, typename TCharTraits = CharTraits<TChar>, typename TAllocator = StandardAllocator<strsize>>
    class BasicString
    {
    public:
        using CharType = TChar;
        using CharTraitsType = TCharTraits;
        using AllocatorType = typename TAllocator::template ElementAllocator<TChar>;
        using Pointer = CharType*;
        using ConstPointer = const CharType*;

    public:
        BasicString() = default;

        BasicString(CharType ch);

        BasicString(ConstPointer ch, strsize len = -1);

        BasicString(strsize count, CharType ch);

        BasicString(const BasicString& other) noexcept = default;

        BasicString(BasicString&& other) noexcept = default;

        BasicString(const BasicString& other, strsize extraSlack);

        ~BasicString() = default;

        Pointer Data()
        {
            if (LargeStringEngaged())
            {
                return Pair.Second().Ptr;
            }

            return Pair.Second().InlineBuffer;
        }

        ConstPointer Data() const
        {
            if (LargeStringEngaged())
            {
                return Pair.Second().Ptr;
            }

            return Pair.Second().InlineBuffer;
        }

        strsize Length() const;

        strsize Capacity() const;

    protected:
        AllocatorType& GetAllocator()
        {
            return Pair.First();
        }

        strsize Size() const
        {
            return Pair.Second().Size;
        }

        bool LargeStringEngaged() const
        {
            return StringVal<CharType>::INLINE_SIZE < Pair.Second().MaxSize;
        }

        template <typename... Args>
        void EmplaceBack(Args&&... args)
        {
            strsize index = AddUnconstructElement(1);
            new(Data() + index) CharType(Forward<Args>(args)...);
        }

        void Add(ConstPointer elements, strsize count)
        {
            strsize index = AddUnconstructElement(count);
            CharTraitsType::Copy(Data() + index, elements, count);
        }
    public:
        void Reserve(strsize capacity);

    private:
        strsize AddUnconstructElement(strsize count)
        {
            ENSURE(count > 0);
            strsize index = Pair.Second().Size;
            Pair.Second().Size += count;
            if (Pair.Second().Size > Capacity())
            {
                Expansion();
            }
            return index;
        }

        void Expansion(strsize destSize = -1)
        {
            destSize = destSize >= 0 ? destSize : Pair.Second().Size;
            strsize newCapacity = CalculateGrowth(destSize);
            ENSURE(destSize <= newCapacity);
            Reserve(newCapacity);
        }

        strsize CalculateGrowth(const strsize newSize) const
        {
            strsize oldCapacity = Pair.Second().MaxSize;
            strsize max = NumericLimits<strsize>::Max();

            if (oldCapacity > max - oldCapacity / 2)
            {
                return max;
            }

            const strsize geometric = oldCapacity + oldCapacity / 2;

            if (geometric < newSize)
            {
                return newSize;
            }

            return geometric;
        }

        void BecomeSmall();

        void BecomeLarge(strsize capacity);

    private:
        CompressedPair<AllocatorType, StringVal<CharType>> Pair;
    };

    template <typename TChar, typename TCharTraits, typename TAllocator>
    BasicString<TChar, TCharTraits, TAllocator>::BasicString(CharType ch)
    {
        Reserve(2);
        EmplaceBack(ch);
        EmplaceBack(CharType());
    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    BasicString<TChar, TCharTraits, TAllocator>::BasicString(BasicString::ConstPointer ch, strsize len)
    {
        len = len < 0 ? CharTraitsType::Length(ch) : len;
        Reserve(len + 1);
        Add(ch, len);
        EmplaceBack(CharType());
    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    BasicString<TChar, TCharTraits, TAllocator>::BasicString(strsize count, CharType ch)
    {
        ENSURE(count > 0);
        Reserve(count + 1);
        strsize index = AddUnconstructElement(count + 1);
        for (strsize i = 0; i < count; ++index, ++i)
        {
            new(Data() + index) CharType(ch);
        }
        new(Data() + count) CharType();
    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    void BasicString<TChar, TCharTraits, TAllocator>::Reserve(strsize capacity)
    {
        if (capacity < Size() || capacity == Capacity())
        {
            return;
        }

        if (LargeStringEngaged() && capacity <= StringVal<CharType>::INLINE_SIZE)
        {
            BecomeSmall();
        }
        else if (!LargeStringEngaged() && capacity > Capacity())
        {
            BecomeLarge(capacity);
        }
    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    void BasicString<TChar, TCharTraits, TAllocator>::BecomeSmall()
    {
        Pointer ptr = Pair.Second().Ptr;
        CharTraitsType::Copy(Pair.Second().InlineBuffer, ptr, Size());
        std::destroy_at(ptr);
        GetAllocator().Deallocate(ptr, Size());
        Pair.Second().MaxSize = StringVal<CharType>::INLINE_SIZE;
    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    void BasicString<TChar, TCharTraits, TAllocator>::BecomeLarge(strsize capacity)
    {
        Pointer ptr = GetAllocator().Allocate(capacity);
        CharTraitsType::Copy(ptr, Pair.Second().InlineBuffer, Size());
        Pair.Second().Ptr = ptr;
        Pair.Second().MaxSize = capacity;
    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    strsize BasicString<TChar, TCharTraits, TAllocator>::Length() const
    {
        return Size() > 0 ? Size() - 1 : 0;
    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    strsize BasicString<TChar, TCharTraits, TAllocator>::Capacity() const
    {
        return Pair.Second().MaxSize;
    }

    using String = BasicString<char>;
}
