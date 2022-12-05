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

        bool LargeStringEngaged() const
        {
            return StringVal<CharType>::INLINE_SIZE < MaxSize;
        }

        TChar* GetPtr()
        {
            if (LargeStringEngaged())
            {
                return UBuffer.Ptr;
            }

            return UBuffer.InlineBuffer;
        }

        const TChar* GetPtr() const
        {
            if (LargeStringEngaged())
            {
                return UBuffer.Ptr;
            }

            return UBuffer.InlineBuffer;
        }

        using CharType = TChar;
        static constexpr int32 INLINE_SIZE = (16 / sizeof(CharType) < 1) ? 1 : (16 / sizeof(CharType));

        strsize Size{ 0 };
        strsize MaxSize{ INLINE_SIZE };

        union UBufferType
        {
            UBufferType() : Ptr() {}
            ~UBufferType() {}

            CharType* Ptr;
            CharType InlineBuffer[INLINE_SIZE];
        } UBuffer;
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

        explicit BasicString(CharType ch);

        explicit BasicString(ConstPointer ch, strsize len = -1);

        BasicString(strsize count, CharType ch);

        BasicString(const BasicString& other);

        BasicString(BasicString&& other) noexcept;

        BasicString(const BasicString& other, strsize extraSlack);

        ~BasicString();

        BasicString& operator= (const BasicString& other) = default;

        BasicString& operator= (BasicString&& other) = default;

        BasicString& operator= (const Pointer raw);

        Pointer Data()
        {
            return Pair.Second().GetPtr();
        }

        ConstPointer Data() const
        {
            return Pair.Second().GetPtr();
        }

        strsize Length() const;

        strsize Capacity() const;

        void Clear()
        {
            Eos(0);
        }

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
            return Pair.Second().LargeStringEngaged();
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

        void Reserve(strsize capacity);

        void Eos(strsize offset);

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
        static constexpr auto INLINE_SIZE = StringVal<CharType>::INLINE_SIZE;
        static constexpr bool CAN_MEMORY_COPY = std::is_trivial_v<Pointer>;

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
    BasicString<TChar, TCharTraits, TAllocator>::BasicString(const BasicString& other) noexcept
        : Pair(OneArgPlaceholder(), MoveTemp(other.GetAllocator()))
    {
        if constexpr (CAN_MEMORY_COPY)
        {
            Pair.Second().Size = other.Pair.Second().Size;
            Pair.Second().MaxSize = other.Pair.Second().MaxSize;
        }
    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    BasicString<TChar, TCharTraits, TAllocator>::BasicString(BasicString&& other) noexcept
        : Pair(OneArgPlaceholder(), MoveTemp(other.GetAllocator()))
    {

    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    BasicString<TChar, TCharTraits, TAllocator>::~BasicString()
    {
        if (LargeStringEngaged())
        {
            const Pointer ptr = Pair.Second().UBuffer.Ptr;
            auto& allocator = GetAllocator();
            std::destroy_at(Pair.Second().UBuffer.Ptr);
            allocator.Deallocate(ptr, Size());
        }

        Pair.Second().Size = 1;
        Pair.Second().MaxSize = INLINE_SIZE;
        CharTraitsType::Assign(Pair.Second().UBuffer.InlineBuffer[0], CharType());
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
    void BasicString<TChar, TCharTraits, TAllocator>::Eos(strsize offset)
    {
        Pair.Second().Size = offset + 1;
        CharTraitsType::Assign(Pair.Second().GetPtr()[offset], 1, CharType());
    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    void BasicString<TChar, TCharTraits, TAllocator>::BecomeSmall()
    {
        Pointer ptr = Pair.Second().UBuffer.Ptr;
        CharTraitsType::Copy(Pair.Second().UBuffer.InlineBuffer, ptr, Size());
        std::destroy_at(ptr);
        GetAllocator().Deallocate(ptr, Size());
        Pair.Second().MaxSize = INLINE_SIZE;
    }

    template <typename TChar, typename TCharTraits, typename TAllocator>
    void BasicString<TChar, TCharTraits, TAllocator>::BecomeLarge(strsize capacity)
    {
        Pointer ptr = GetAllocator().Allocate(capacity);
        CharTraitsType::Copy(ptr, Pair.Second().UBuffer.InlineBuffer, Size());
        Pair.Second().UBuffer.Ptr = ptr;
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
