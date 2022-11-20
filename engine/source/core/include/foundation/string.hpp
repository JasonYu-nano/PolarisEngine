#pragma once

#include "foundation/details/compressed_pair.hpp"
#include "foundation/char_traits.hpp"
#include "memory/allocator_policies.hpp"

namespace Engine
{
    template <typename TChar>
    struct StringVal
    {
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

    template <typename TChar, typename TCharTraits = CharTraits<TChar>, typename TAllocator = DefaultAllocator>
    class BasicString
    {
    public:
        using CharType = TChar;
        using CharTraitsType = TCharTraits;
        using AllocatorType = typename TAllocator::template ElementAllocator<TChar>;

        static constexpr int32 INLINE_SIZE = (16 / sizeof(CharType) < 1) ? 1 : (16 / sizeof(CharType));

    public:
        BasicString() = default;

        BasicString(CharType ch);

        BasicString(CharType* ch, strsize len = -1);

        BasicString(strsize count, CharType ch);

        BasicString(const BasicString& other) noexcept = default;

        BasicString(BasicString&& other) noexcept = default;

        BasicString(const BasicString& other, strsize extraSlack);

        ~BasicString() = default;

        CharType* Data()
        {
            if (LargeStringEngaged())
            {
                return Pair.Second().Ptr;
            }

            return Pair.Second().InlineBuffer;
        }

        const CharType* Data() const
        {
            if (LargeStringEngaged())
            {
                return Pair.Second().Ptr;
            }

            return Pair.Second().InlineBuffer;
        }

    protected:
        bool LargeStringEngaged() const
        {
            return Pair.Second().INLINE_SIZE < Pair.Second().MaxSize;
        }

        template <typename... Args>
        void EmplaceBack(Args&&... args)
        {
            strsize index = AddUnconstructElement(1);
            new(Data() + index) CharType(Forward<Args>(args)...);
        }

        void Reserve(strsize capacity);

    private:
        strsize AddUnconstructElement(strsize count)
        {
            ENSURE(count > 0);
            strsize index = Pair.Second().Size;
            Pair.Second().Size += count;
            if (Pair.Second().Size > Pair.Second().MaxSize)
            {
                Expansion();
            }
            return index;
        }

        void Expansion(strsize destSize = -1)
        {
            destSize = destSize >= 0 ? destSize : Pair.Second().Size;
            Pair.Second().MaxSize = CalculateGrowth(destSize);
            ENSURE(destSize <= Pair.Second().MaxSize);
            Allocator.Resize(MaxSize);
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

        void BecomeLarge();

    private:
        CompressedPair<AllocatorType, StringVal<CharType>> Pair;
    };

    template <typename TChar, typename TCharTraits, typename TAllocator>
    BasicString<TChar, TCharTraits, TAllocator>::BasicString(CharType ch)
    {
        Reserve(2);
        EmplaceBack(ch);
        EmplaceBack((CharType)'\0');
    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    void BasicString<TChar, TCharTraits, TAllocator>::Reserve(strsize capacity)
    {
        if (capacity < Size || capacity == MaxSize)
        {
            return;
        }

        if (LargeStringEngaged() && capacity < INLINE_SIZE - 1)
        {
            // TODO: Become small
        }
        else if (!LargeStringEngaged() && capacity > MaxSize)
        {
            //  TODO: Become large
        }

        if (capacity > Size && capacity > INLINE_SIZE && capacity != MaxSize)
        {
            bool needCopy = MaxSize <= INLINE_SIZE;
            MaxSize = capacity;
            CharType smallString[INLINE_SIZE];
            Memory::Memcpy(smallString, InlineBuffer, sizeof(CharType) * INLINE_SIZE);
            Allocator.Resize(capacity);
        }
    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    void BasicString<TChar, TCharTraits, TAllocator>::BecomeSmall()
    {

    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    void BasicString<TChar, TCharTraits, TAllocator>::BecomeLarge()
    {

    }

    class String : protected BasicString<char> {};
}
