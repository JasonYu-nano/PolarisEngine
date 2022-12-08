#pragma once

#include "foundation/details/compressed_pair.hpp"
#include "foundation/char_traits.hpp"
#include "memory/allocator_policies.hpp"

namespace Engine
{
    template <typename Elem, typename Traits>
    struct StringVal
    {
        StringVal() {};

        ~StringVal() {};

        bool LargeStringEngaged() const
        {
            return INLINE_SIZE < MaxSize;
        }

        Elem* GetPtr()
        {
            if (LargeStringEngaged())
            {
                return UB.Ptr;
            }

            return UB.Buffer;
        }

        const Elem* GetPtr() const
        {
            if (LargeStringEngaged())
            {
                return UB.Ptr;
            }

            return UB.Buffer;
        }

        using CharType = Elem;
        static constexpr int32 INLINE_SIZE = (16 / sizeof(CharType) < 1) ? 1 : (16 / sizeof(CharType));

        typename Traits::SizeType Size{ 1 };
        typename Traits::SizeType MaxSize{ INLINE_SIZE };

        union _UB
        {
            _UB() { Traits::Assign(Buffer[0], CharType()); }
            ~_UB() {}

            CharType* Ptr;
            CharType Buffer[INLINE_SIZE];
        } UB;
    };

    template <typename Elem, typename Traits = CharTraits<Elem>, typename Alloc = StandardAllocator<typename Traits::SizeType>>
    class BasicString
    {
    public:
        using CharType = Elem;
        using CharTraits = Traits;
        using SizeType = typename CharTraits::SizeType;
        using AllocatorType = typename Alloc::template ElementAllocator<Elem>;
        using Pointer = CharType*;
        using ConstPointer = const CharType*;

    public:
        BasicString() = default;

        explicit BasicString(CharType ch);

        explicit BasicString(ConstPointer ch, SizeType len = -1);

        BasicString(SizeType count, CharType ch);

        BasicString(const BasicString& other);

        BasicString(BasicString&& other) noexcept;

        BasicString(const BasicString& other, SizeType extraSlack);

        ~BasicString();

        BasicString& operator= (const BasicString& other);

        BasicString& operator= (BasicString&& other) noexcept ;

        BasicString& operator= (const CharType* raw);

        bool operator== (const BasicString& other) const;

        bool operator!= (const BasicString& other) const;

        bool operator== (const BasicStringView<CharType>& other) const;

        bool operator!= (const BasicStringView<CharType>& other) const;

        CharType operator[] (SizeType index) const
        {
            ENSURE(index < Length());
            return *(Pair.Second().GetPtr() + index);
        }

        CharType operator[] (SizeType index)
        {
            ENSURE(index < Length());
            return *(Pair.Second().GetPtr() + index);
        }

        explicit operator BasicStringView<CharType>() const
        {
            return BasicStringView<CharType>(Data(), Length());
        }

        Pointer Data()
        {
            return Pair.Second().GetPtr();
        }

        ConstPointer Data() const
        {
            return Pair.Second().GetPtr();
        }

        SizeType Length() const;

        SizeType Capacity() const;

        NODISCARD bool Empty() const
        {
            return Size() <= 1;
        }

        void Resize(SizeType len);

        void Truncate(SizeType pos)
        {
            pos = pos < 0 ? pos + Length() : pos;
            if (pos >= 0 && pos < Length())
            {
                Pair.Second().Size = pos + 1;
                CharTraits::Assign(Pair.Second().GetPtr() + pos, 1, CharType());
            }
        }

        BasicString Slices(SizeType pos, SizeType num) const
        {
            pos = pos < 0 ? pos + Length() : pos;
            if (pos >= 0 && num >= 0 && pos + num <= Length())
            {
                return BasicString(Data() + pos, num);
            }

            return BasicString();
        }

        bool StartsWith(const BasicStringView<CharType>& view, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        bool StartsWith(const BasicString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return StartsWith(static_cast<BasicStringView<CharType>>(str), cs);
        }

        bool StartsWith(CharType ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return StartsWith(BasicStringView<CharType>(ch, 1), cs);
        }

        bool EndsWith(const BasicStringView<CharType>& view, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        bool EndsWith(const BasicString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return EndsWith(static_cast<BasicStringView<CharType>>(str), cs);
        }

        inline bool EndsWith(CharType ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return EndsWith(BasicStringView<CharType>(ch, 1), cs);
        }

        BasicString& Append(const BasicStringView<CharType>& view);

        BasicString& Append(const BasicString& str)
        {
            return Append(static_cast<BasicStringView<CharType>>(str));
        }

        BasicString& Append(CharType ch)
        {
            return Append(BasicStringView<CharType>(ch, 1));
        }

        BasicString& Prepend(const BasicStringView<CharType>& view);

        BasicString& Prepend(const BasicString& str)
        {
            return Prepend(static_cast<BasicStringView<CharType>>(str));
        }

        BasicString& Prepend(CharType ch)
        {
            return Prepend(BasicStringView<CharType>(ch, 1));
        }

        BasicString& Insert(SizeType index, const BasicStringView<CharType>& view)
        {
            SizeType oldLen = Length();
            if (index > Length())
            {
                return *this;
            }

            SizeType len = view.Length();
            InsertUnconstructElement(index, len);
            CharTraits::Copy(Data() + index, view.Data(), view.Length());
            return *this;
        }

        BasicString& Insert(SizeType index, const BasicString& str)
        {
            return Insert(index, static_cast<BasicStringView<CharType>>(str));
        }

        BasicString& Insert(SizeType index, CharType ch)
        {
            return Insert(index, BasicStringView<CharType>(ch, 1));
        }

        void Clear()
        {
            Truncate(0);
        }

    protected:
        AllocatorType& GetAllocator()
        {
            return Pair.First();
        }

        const AllocatorType& GetAllocator() const
        {
            return Pair.First();
        }

        SizeType Size() const
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
            SizeType index = AddUnconstructElement(1);
            new(Data() + index) CharType(Forward<Args>(args)...);
        }

        void Reserve(SizeType capacity);

        void Invalidate();

        void MoveAssign(BasicString&& right);

        void CopyAssign(const BasicString& right);

        void CopyAssign(const StringView& right);

    private:
        SizeType AddUnconstructElement(SizeType count)
        {
            auto& val = Pair.Second();
            ENSURE(count > 0 && val.Size > 0);
            SizeType index = val.Size - 1;
            SizeType newSize = val.Size + count;
            if (newSize > Capacity())
            {
                Expansion(newSize);
            }

            val.Size = newSize;
            return index;
        }

        void InsertUnconstructElement(SizeType index, SizeType count)
        {
            ENSURE(index >= 0 && count > 0 && index <= Size());
            auto& val = Pair.Second();
            SizeType oldSize = val.Size;
            SizeType newSize = val.Size + count;
            if (newSize > val.MaxSize)
            {
                Expansion(newSize);
            }

            val.Size = newSize;
            CharType* src = val.GetPtr() + index;
            Memory::Memmove(src + count, src, (oldSize - index) * sizeof(CharType));
        }

        void Expansion(SizeType destSize)
        {
            SizeType newCapacity = CalculateGrowth(destSize);
            ENSURE(destSize <= newCapacity);
            Reserve(newCapacity);
        }

        SizeType CalculateGrowth(const SizeType newSize) const
        {
            SizeType oldCapacity = Pair.Second().MaxSize;
            SizeType max = NumericLimits<SizeType>::Max();

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

        void BecomeSmall();

        void BecomeLarge(SizeType capacity);

        int32 Compare(const BasicString& other, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        int32 Compare(const BasicStringView<CharType>& other, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

    private:
        static constexpr auto INLINE_SIZE = StringVal<CharType, CharTraits>::INLINE_SIZE;
        static constexpr bool CAN_MEMORY_COPY = std::is_trivial_v<Pointer>;

        CompressedPair<AllocatorType, StringVal<CharType, CharTraits>> Pair;
    };

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>::BasicString(CharType ch)
    {
        Append(BasicStringView<CharType>(std::addressof(ch), 1));
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>::BasicString(BasicString::ConstPointer ch, SizeType len)
    {
        len = len < 0 ? CharTraits::Length(ch) : len;
        Append(BasicStringView<CharType>(ch, len));
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>::BasicString(SizeType count, CharType ch)
    {
        ENSURE(count > 0);
        SizeType index = AddUnconstructElement(count);
        for (SizeType i = 0; i < count; ++i)
        {
            CharTraits::Assign(Data()[i], ch);
        }
        CharTraits::Assign(Data()[index + count], CharType());
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>::BasicString(const BasicString& other)
        : Pair(OneArgPlaceholder(), other.GetAllocator())
    {
        CopyAssign(other);
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>::BasicString(BasicString&& other) noexcept
        : Pair(OneArgPlaceholder(), MoveTemp(other.GetAllocator()))
    {
        MoveAssign(Forward<BasicString&&>(other));
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>::~BasicString()
    {
        if (LargeStringEngaged())
        {
            const Pointer ptr = Pair.Second().UB.Ptr;
            auto& allocator = GetAllocator();
            std::destroy_at(Pair.Second().UB.Ptr);
            allocator.Deallocate(ptr, Size());
        }

        Invalidate();
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::operator=(const BasicString& other)
    {
        Pair.First() = other.GetAllocator();
        CopyAssign(other);
        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::operator=(BasicString&& other) noexcept
    {
        Pair.First() = MoveTemp(other.GetAllocator());
        MoveAssign(other);
        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::operator=(const CharType* raw)
    {
        CopyAssign(StringView(raw));
        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::Reserve(SizeType capacity)
    {
        if (capacity < Size() || capacity == Capacity())
        {
            return;
        }

        if (LargeStringEngaged() && capacity <= INLINE_SIZE)
        {
            BecomeSmall();
        }
        else if (!LargeStringEngaged() && capacity > Capacity())
        {
            BecomeLarge(capacity);
        }
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::BecomeSmall()
    {
        Pointer ptr = Pair.Second().UB.Ptr;
        CharTraits::Copy(Pair.Second().UB.Buffer, ptr, Size());
        std::destroy_at(ptr);
        GetAllocator().Deallocate(ptr, Size());
        Pair.Second().MaxSize = INLINE_SIZE;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::BecomeLarge(SizeType capacity)
    {
        Pointer ptr = GetAllocator().Allocate(capacity);
        CharTraits::Copy(ptr, Pair.Second().UB.Buffer, Size());
        Pair.Second().UB.Ptr = ptr;
        Pair.Second().MaxSize = capacity;
    }

    template <typename Elem, typename Traits, typename Alloc>
    typename BasicString<Elem, Traits, Alloc>::SizeType BasicString<Elem, Traits, Alloc>::Length() const
    {
        return Size() > 0 ? Size() - 1 : 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    typename BasicString<Elem, Traits, Alloc>::SizeType BasicString<Elem, Traits, Alloc>::Capacity() const
    {
        return Pair.Second().MaxSize;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::Invalidate()
    {
        auto& myVal = Pair.Second();
        myVal.Size = 1;
        myVal.MaxSize = INLINE_SIZE;
        myVal.UB.Buffer[0] = CharType();
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::MoveAssign(BasicString&& right)
    {
        ENSURE(std::addressof(*this) != std::addressof(right));
        auto& leftVal = Pair.Second();
        auto& rightVal = right.Pair.Second();
        int32 size = rightVal.Size;
        ENSURE(size >= 0);
        Reserve(rightVal.MaxSize);

        if constexpr (CAN_MEMORY_COPY)
        {
            const auto lhsData = reinterpret_cast<uint8*>(std::addressof(leftVal));
            const auto rhsData = reinterpret_cast<const uint8*>(std::addressof(rightVal));
            Memory::Memcpy(lhsData, rhsData, size);
            right.Invalidate();
            return;
        }

        if (right.LargeStringEngaged())
        {
            leftVal.UB.Ptr = rightVal.UB.Ptr;
            rightVal.UB.Ptr = nullptr;
        }
        else
        {
            CharTraits::Copy(leftVal.UB.Buffer, rightVal.UB.Buffer, size);
        }

        leftVal.Size = rightVal.Size;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::CopyAssign(const BasicString& right)
    {
        ENSURE(std::addressof(*this) != std::addressof(right));
        auto& leftVal = Pair.Second();
        auto& rightVal = right.Pair.Second();
        int32 size = rightVal.Size;
        ENSURE(size >= 0);
        Reserve(rightVal.MaxSize);

        CharTraits::Copy(leftVal.GetPtr(), rightVal.GetPtr(), size);
        leftVal.Size = rightVal.Size;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::CopyAssign(const StringView& right)
    {
        ENSURE(std::addressof(*this) != std::addressof(*right.Data()));
        auto& leftVal = Pair.Second();
        auto rightVal = right.Data();
        int32 size = right.Length() + 1;
        ENSURE(size >= 0);
        Reserve(size);

        CharTraits::Copy(leftVal.GetPtr(), rightVal, size);
        leftVal.Size = size;
    }

    template <typename Elem, typename Traits, typename Alloc>
    int32 BasicString<Elem, Traits, Alloc>::Compare(const BasicString& other, ECaseSensitivity cs) const
    {
        return CharTraits::Compare(Data(), Length(), other.Data(), other.Length());
    }

    template <typename Elem, typename Traits, typename Alloc>
    int32 BasicString<Elem, Traits, Alloc>::Compare(const BasicStringView<Elem>& other, ECaseSensitivity cs) const
    {
        return CharTraits::Compare(Data(), Length(), other.Data(), other.Length());
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::operator==(const BasicString& other) const
    {
        return Compare(other) == 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::operator!=(const BasicString& other) const
    {
        return Compare(other) != 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::operator==(const BasicStringView<Elem>& other) const
    {
        return Compare(other) == 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::operator!=(const BasicStringView<Elem>& other) const
    {
        return Compare(other) != 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::StartsWith(const BasicStringView<CharType>& view, ECaseSensitivity cs) const
    {
        SizeType compareLen = view.Length();
        if (Empty() || Length() < compareLen)
        {
            return false;
        }

        return CharTraits::Compare(Data(), view.Data(), compareLen) == 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::EndsWith(const BasicStringView<CharType>& view, ECaseSensitivity cs) const
    {
        strsize compareLen = view.Length();
        if (Empty() || Length() < compareLen)
        {
            return false;
        }

        return CharTraits::Compare(Data() + Length() - compareLen, view.Data(), compareLen) == 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::Append(const BasicStringView<CharType>& view)
    {
        if (view.Null() || view.Empty())
        {
            return *this;
        }

        SizeType len = view.Length();
        SizeType index = AddUnconstructElement(len);
        CharTraits::Copy(Data() + index, view.Data(), len);
        CharTraits::Assign(*(Data() + index + len), CharType());
        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::Prepend(const BasicStringView<CharType>& view)
    {
        return Insert(0, view);
    }

    using String = BasicString<char>;
}
