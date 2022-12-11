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

        BasicString(CharType ch);

        BasicString(ConstPointer ch, SizeType len = -1);

        BasicString(SizeType count, CharType ch);

        BasicString(const BasicString& other);

        BasicString(BasicString&& other) noexcept;

        ~BasicString();

        BasicString& operator= (const BasicString& other);

        BasicString& operator= (BasicString&& other) noexcept ;

        BasicString& operator= (const CharType* raw);

        bool operator== (const BasicString& other) const;

        bool operator!= (const BasicString& other) const;

        bool operator== (const CharType* other) const;

        bool operator!= (const CharType* other) const;

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

        BasicString& Append(const CharType* str)
        {
            return Append(BasicStringView<CharType>(str));
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

        BasicString& Prepend(const CharType* str)
        {
            return Prepend(BasicStringView<CharType>(str));
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

        BasicString& Insert(SizeType index, const CharType* str)
        {
            return Insert(index, BasicStringView<CharType>(str));
        }

        BasicString& Insert(SizeType index, CharType ch)
        {
            return Insert(index, BasicStringView<CharType>(ch, 1));
        }

        BasicString& Remove(SizeType pos, SizeType num);

        BasicString& Remove(const BasicStringView<CharType>& view, ECaseSensitivity cs = ECaseSensitivity::Sensitive);

        BasicString& Remove(const BasicString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive)
        {
            return Remove(static_cast<BasicStringView<CharType>>(str), cs);
        }

        BasicString& Remove(const CharType* str, ECaseSensitivity cs = ECaseSensitivity::Sensitive)
        {
            return Remove(BasicStringView<CharType>(str), cs);
        }

        BasicString& Remove(CharType ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive)
        {
            return Remove(BasicStringView<CharType>(std::addressof(ch), 1), cs);
        }

        BasicString& Replace(const BasicStringView<CharType>& before, const BasicStringView<CharType>& after, ECaseSensitivity cs = ECaseSensitivity::Sensitive)
        {
            ReplaceHelper(*this, before, after, cs);
            return *this;
        }

        BasicString& Replace(const BasicString& before, const BasicString& after, ECaseSensitivity cs = ECaseSensitivity::Sensitive)
        {
            return Replace(static_cast<BasicStringView<CharType>>(before), static_cast<BasicStringView<CharType>>(after), cs);
        }

        BasicString& Replace(const CharType* before, const CharType* after, ECaseSensitivity cs = ECaseSensitivity::Sensitive)
        {
            return Replace(BasicStringView<CharType>(before), BasicStringView<CharType>(after), cs);
        }

        /**
         * Removes n characters from the end of the string.
         * If n is greater than or equal to Length(), the result is an empty string;
         * @param n
         */
        void Chop(SizeType n);

        BasicString Chopped(SizeType n);

        SizeType IndexOf(const BasicStringView<CharType>& view, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return FindStringHelper(static_cast<BasicStringView<CharType>>(*this), 0, view, cs);
        }

        SizeType IndexOf(const CharType* str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return IndexOf(BasicStringView<CharType>(str), cs);
        }

        SizeType IndexOf(const BasicString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return IndexOf(static_cast<BasicStringView<CharType>>(str), cs);
        }

        SizeType IndexOf(CharType ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return IndexOf(BasicStringView<CharType>(std::addressof(ch), 1), cs);
        }

        SizeType IndexOfAny(const BasicStringView<CharType>& view, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return FindAnyCharHelper(static_cast<BasicStringView<CharType>>(*this), 0, view, cs);
        }

        SizeType IndexOfAny(const BasicString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return IndexOfAny(static_cast<BasicStringView<CharType>>(str), cs);
        }

        SizeType IndexOfAny(const CharType* str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return IndexOfAny(BasicStringView<CharType>(str), cs);
        }

        SizeType LastIndexOf(const BasicStringView<CharType>& view, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return FindLastHelper(static_cast<BasicStringView<CharType>>(*this), -1, view, cs);
        }

        SizeType LastIndexOf(const BasicString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return LastIndexOf(static_cast<BasicStringView<CharType>>(str), cs);
        }

        SizeType LastIndexOf(const CharType* str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return LastIndexOf(BasicStringView<CharType>(str), cs);
        }

        SizeType LastIndexOf(CharType ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return LastIndexOf(BasicStringView<CharType>(std::addressof(ch), 1), cs);
        }

        SizeType LastIndexOfAny(const BasicStringView<CharType>& view, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return FindLastAnyCharHelper(static_cast<BasicStringView<CharType>>(*this), 0, view, cs);
        }

        SizeType LastIndexOfAny(const CharType* str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return LastIndexOfAny(BasicStringView<CharType>(str), cs);
        }

        SizeType LastIndexOfAny(const BasicString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return LastIndexOfAny(static_cast<BasicStringView<CharType>>(str), cs);
        }

        bool Contains(const BasicStringView<CharType>& view, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return FindStringHelper(static_cast<BasicStringView<CharType>>(*this), 0, view, cs) >= 0;
        }

        bool Contains(const CharType* str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return Contains(BasicStringView<CharType>(str), cs);
        }

        bool Contains(const BasicString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return Contains(static_cast<BasicStringView<CharType>>(str), cs);
        }

        bool Contains(UChar ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return Contains(BasicStringView<CharType>(std::addressof(ch), 1), cs);
        }

        int32 Count(const BasicStringView<CharType>& view, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            SizeType num = 0;
            SizeType i = -1;
            while ((i = FindStringHelper(static_cast<BasicStringView<CharType>>(*this), i + 1, view, cs)) != -1)
            {
                ++num;
            }
            return num;
        }

        int32 Count(const BasicString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return Count(static_cast<BasicStringView<CharType>>(str), cs);
        }

        int32 Count(const CharType* str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return Count(BasicStringView<CharType>(str), cs);
        }

        int32 Count(CharType ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return Count(BasicStringView<CharType>(std::addressof(ch), 1), cs);
        }

        BasicString& Fill(CharType ch, int32 num = -1);

        BasicString	Repeated(int32 times) const;

        BasicString Trimmed() const;

        bool IsUpperAscii() const;

        bool IsLowerAscii() const;

        void ToUpperAscii();

        void ToLowerAscii();

        DynamicArray<BasicString> Split(const BasicStringView<CharType>& sep, ESplitBehavior behavior = ESplitBehavior::KeepEmptyParts, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        DynamicArray<BasicString> Split(const BasicString& sep, ESplitBehavior behavior = ESplitBehavior::KeepEmptyParts, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return Split(static_cast<BasicStringView<CharType>>(sep), behavior, cs);
        }

        DynamicArray<BasicString> Split(const CharType* sep, ESplitBehavior behavior = ESplitBehavior::KeepEmptyParts, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return Split(BasicStringView<CharType>(sep), behavior, cs);
        }

        DynamicArray<BasicString> Split(CharType sep, ESplitBehavior behavior = ESplitBehavior::KeepEmptyParts, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return Split(BasicStringView<CharType>(std::addressof(sep), 1), behavior, cs);
        }

        DynamicArray<BasicString> SplitAny(const BasicStringView<CharType>& sep, ESplitBehavior behavior = ESplitBehavior::KeepEmptyParts, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        DynamicArray<BasicString> SplitAny(const BasicString& sep, ESplitBehavior behavior = ESplitBehavior::KeepEmptyParts, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return SplitAny(static_cast<BasicStringView<CharType>>(sep), behavior, cs);
        }

        DynamicArray<BasicString> SplitAny(const CharType* sep, ESplitBehavior behavior = ESplitBehavior::KeepEmptyParts, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return SplitAny(BasicStringView<CharType>(sep), behavior, cs);
        }

        DynamicArray<BasicString> SplitAny(CharType sep, ESplitBehavior behavior = ESplitBehavior::KeepEmptyParts, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const
        {
            return SplitAny(BasicStringView<CharType>(std::addressof(sep), 1), behavior, cs);
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

        void CopyAssign(const BasicStringView<CharType>& right);

        static SizeType FindStringHelper(const BasicStringView<CharType>& haystack, SizeType from, const BasicStringView<CharType>& needle, ECaseSensitivity cs);

        static SizeType FindAnyCharHelper(const BasicStringView<CharType>& haystack, SizeType from, const BasicStringView<CharType>& needle, ECaseSensitivity cs);

        static SizeType FindLastHelper(const BasicStringView<CharType>& haystack, SizeType from, const BasicStringView<CharType>& needle, ECaseSensitivity cs);

        static SizeType FindLastAnyCharHelper(const BasicStringView<CharType>& haystack, SizeType from, const BasicStringView<CharType>& needle, ECaseSensitivity cs);

        static void ReplaceHelper(BasicString& source, const BasicStringView<CharType>& before, const BasicStringView<CharType>& after, ECaseSensitivity cs = ECaseSensitivity::Sensitive);

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

        void DestructElements(CharType* elems, SizeType count)
        {
            bool needDestroy = LargeStringEngaged();
            while (count)
            {
                if (needDestroy)
                {
                    std::destroy_at(elems);
                }
                ++elems;
                --count;
            }
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
        MoveAssign(Forward<BasicString>(other));
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
        auto& val = Pair.Second();
        if (capacity < val.Size || capacity <= val.MaxSize)
        {
            return;
        }

        if (!LargeStringEngaged())
        {
            BecomeLarge(capacity);
            return;
        }

        auto& alloc = GetAllocator();
        Pointer ptr = alloc.Allocate(capacity);
        Pointer oldPtr = val.UB.Ptr;
        SizeType oldSize = val.Size;
        CharTraits::Copy(ptr, oldPtr, oldSize);
        val.UB.Ptr = ptr;
        val.MaxSize = capacity;
        alloc.Deallocate(oldPtr, oldSize);
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
            Memory::Memcpy(lhsData, rhsData, sizeof(decltype(leftVal)));
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
        ENSURE(this->Data() != right.Data());
        auto& leftVal = Pair.Second();
        auto& rightVal = right.Pair.Second();
        SizeType size = rightVal.Size;
        ENSURE(size >= 0);
        Reserve(rightVal.MaxSize);

        CharTraits::Copy(leftVal.GetPtr(), rightVal.GetPtr(), size);
        leftVal.Size = rightVal.Size;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::CopyAssign(const BasicStringView<CharType>& right)
    {
        ENSURE(this->Data() != right.Data());
        auto& leftVal = Pair.Second();
        SizeType len = right.Length();
        ENSURE(len >= 0);
        Reserve(len + 1);

        CharTraits::Copy(leftVal.GetPtr(), right.Data(), len);
        CharTraits::Assign(leftVal.GetPtr()[len], CharType());
        leftVal.Size = len + 1;
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
    bool BasicString<Elem, Traits, Alloc>::operator==(const CharType* other) const
    {
        return Compare(BasicStringView<CharType>(other)) == 0;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::operator!=(const CharType* other) const
    {
        return Compare(BasicStringView<CharType>(other)) != 0;
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
        SizeType compareLen = view.Length();
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

    template <typename Elem, typename Traits, typename Alloc>
    typename BasicString<Elem, Traits, Alloc>::SizeType BasicString<Elem, Traits, Alloc>::FindStringHelper(const BasicStringView<CharType>& haystack, SizeType from,
        const BasicStringView<CharType>& needle,
        ECaseSensitivity cs)
    {
        return Private::FindString<CharType, CharTraits, SizeType>(haystack.Data(), haystack.Length(), from, needle.Data(), needle.Length(), cs);
    }

    template <typename Elem, typename Traits, typename Alloc>
    typename BasicString<Elem, Traits, Alloc>::SizeType
    BasicString<Elem, Traits, Alloc>::FindAnyCharHelper(const BasicStringView<CharType>& haystack, SizeType from,
                                                        const BasicStringView<CharType>& needle, ECaseSensitivity cs)
    {
        const CharType* data = haystack.Data();
        for (SizeType idx = from; idx < haystack.Length(); ++idx)
        {
            if (FindStringHelper(needle, 0, BasicStringView<CharType>(data + idx, 1), cs) > -1)
            {
                return idx;
            }
        }
        return -1;
    }

    template <typename Elem, typename Traits, typename Alloc>
    typename BasicString<Elem, Traits, Alloc>::SizeType
    BasicString<Elem, Traits, Alloc>::FindLastHelper(const BasicStringView<CharType>& haystack, SizeType from,
                                                              const BasicStringView<CharType>& needle,
                                                              ECaseSensitivity cs)
    {
        return Private::FindLastString<CharType, CharTraits, SizeType>(haystack.Data(), haystack.Length(), from, needle.Data(), needle.Length(), cs);
    }

    template <typename Elem, typename Traits, typename Alloc>
    typename BasicString<Elem, Traits, Alloc>::SizeType
    BasicString<Elem, Traits, Alloc>::FindLastAnyCharHelper(const BasicStringView<CharType>& haystack, SizeType from,
                                                            const BasicStringView<CharType>& needle,
                                                            ECaseSensitivity cs)
    {
        const CharType* data = haystack.Data();
        for (SizeType idx = haystack.Length() - 1; idx >= from; --idx)
        {
            if (FindStringHelper(needle, 0, BasicStringView<CharType>(data + idx, 1), cs) > -1)
            {
                return idx;
            }
        }
        return -1;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::Chop(SizeType n)
    {

    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc> BasicString<Elem, Traits, Alloc>::Chopped(SizeType n)
    {
        return BasicString();
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::Remove(SizeType pos, SizeType num)
    {
        auto& val = Pair.Second();
        auto& size = val.Size;
        auto len = size - 1;
        if (pos < 0)
        {
            pos += len;
        }
        if (pos < 0 || pos >= len)
        {
        }
        else if (num >= len - pos)
        {
            Truncate(pos);
        }
        else if (num > 0)
        {
            SizeType end = pos + num - 1;
            DestructElements(Data() + pos, num);
            SizeType countToMove = Size() - end - 1;
            if (countToMove)
            {
                Memory::Memmove(Data() + pos, Data() + end + 1, countToMove * sizeof(CharType));
            }
            size -= num;
        }
        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::Remove(const BasicStringView<CharType>& view, ECaseSensitivity cs)
    {
        SizeType from = 0;
        do
        {
            SizeType pos = FindStringHelper(static_cast<BasicStringView<CharType>>(*this), from, view, cs);
            if (pos >= 0)
            {
                Remove(pos, view.Length());
                from = pos;
            }
            else
            {
                break;
            }
        }
        while (true);

        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::ReplaceHelper(BasicString& source, const BasicStringView<CharType>& before,
                                                         const BasicStringView<CharType>& after, ECaseSensitivity cs)
    {
        if (before.Empty())
        {
            return;
        }

        SizeType alen = after.Length();
        SizeType blen = before.Length();
        if (source.Length() < blen)
        {
            return;
        }
        if (before == after)
        {
            return;
        }

        Private::StringMatcher<CharType, CharTraits, SizeType> matcher(before.Data(), blen, cs);

        DynamicArray<SizeType, InlineAllocator<128>> indices;
        SizeType pos = matcher.IndexIn(source.Data(), source.Length(), 0);

        while (pos >= 0)
        {
            indices.Add(pos);
            pos += blen;
            pos = matcher.IndexIn(source.Data(), source.Length(), pos);
        }

        int32 nIndices = indices.Size();
        if (nIndices <= 0)
        {
            return;
        }

        auto& val = source.Pair.Second();
        const CharType* afterData = after.Data();
        if (blen == alen)
        {
            CharType* src = val.GetPtr();
            for (int32 i = 0; i < nIndices; ++i)
            {
                CharTraits::Copy(src + indices[i], afterData, alen);
            }
        }
        else
        {
            SizeType oldSize = val.Size;
            int32 deltaSize = (alen - blen);
            int32 newSize = oldSize + deltaSize * nIndices;

            if (newSize > oldSize)
            {
                source.Reserve(newSize);
            }

            CharType* src = val.GetPtr();
            for (int32 i = nIndices - 1; i >= 0; --i)
            {
                strsize idx = indices[i];
                CharTraits::Copy(src + idx + alen, src + idx + blen, oldSize - idx - blen);
                CharTraits::Copy(src + idx, afterData, alen);
            }

            CharTraits::Assign(src[newSize - 1], CharType());
            val.Size = newSize;
        }
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc>& BasicString<Elem, Traits, Alloc>::Fill(CharType ch, int32 num)
    {
        auto& val = Pair.Second();
        SizeType size = val.Size;
        if (size <= 1 && num <= 0)
        {
            return *this;
        }

        SizeType newSize = num >= 0 ? num + 1 : size;
        Reserve(newSize);
        val.Size = newSize;
        auto ptr = val.GetPtr();

        CharTraits::Assign(ptr[--newSize], CharType());
        while (newSize > 0)
        {
            --newSize;
            CharTraits::Assign(ptr[newSize], ch);
        }
        return *this;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc> BasicString<Elem, Traits, Alloc>::Repeated(int32 times) const
    {
        if (Empty() || times == 1)
        {
            return *this;
        }

        if (times <= 0)
        {
            return BasicString();
        }

        SizeType len = Length();
        SizeType resultSize = len * times;
        BasicString ret;
        ret.Reserve(resultSize + 1);
        if (ret.Capacity() > resultSize)
        {
            const CharType* raw = Data();
            while (times > 0)
            {
                ret.Append(BasicStringView<CharType>(raw, len));
                --times;
            }
        }
        CharTraits::Assign(ret.Pair.Second().GetPtr()[resultSize], CharType());
        return ret;
    }

    template <typename Elem, typename Traits, typename Alloc>
    BasicString<Elem, Traits, Alloc> BasicString<Elem, Traits, Alloc>::Trimmed() const
    {
        const CharType* start = Data();
        const CharType* end = Data() + Length() - 1;

        while (start < end && CharTraits::IsSpace(*start))
        {
            ++start;
        }

        while (start < end && CharTraits::IsSpace(end[-1]))
        {
            --end;
        }
        return BasicString(start, static_cast<SizeType>(end - start));
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::IsUpperAscii() const
    {
        auto& myVal = Pair.Second();
        const CharType* data = myVal.GetPtr();
        for (SizeType i = 0; i < myVal.Size - 1; ++i)
        {
            if (CharTraits::Latin1ToUpper(data[i]) != data[i])
            {
                return false;
            }
        }
        return true;
    }

    template <typename Elem, typename Traits, typename Alloc>
    bool BasicString<Elem, Traits, Alloc>::IsLowerAscii() const
    {
        auto& myVal = Pair.Second();
        const CharType* data = myVal.GetPtr();
        for (SizeType i = 0; i < myVal.Size - 1; ++i)
        {
            if (CharTraits::Latin1ToLower(data[i]) != data[i])
            {
                return false;
            }
        }
        return true;
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::ToUpperAscii()
    {
        auto& myVal = Pair.Second();
        CharType* data = myVal.GetPtr();
        for (SizeType i = 0; i < myVal.Size - 1; ++i)
        {
            data[i] = CharTraits::Latin1ToUpper(data[i]);
        }
    }

    template <typename Elem, typename Traits, typename Alloc>
    void BasicString<Elem, Traits, Alloc>::ToLowerAscii()
    {
        auto& myVal = Pair.Second();
        CharType* data = myVal.GetPtr();
        for (SizeType i = 0; i < myVal.Size - 1; ++i)
        {
            data[i] = CharTraits::Latin1ToLower(data[i]);
        }
    }

    template <typename Elem, typename Traits, typename Alloc>
    DynamicArray<BasicString<Elem, Traits, Alloc>>
    BasicString<Elem, Traits, Alloc>::Split(const BasicStringView<CharType>& sep, ESplitBehavior behavior,
                                            ECaseSensitivity cs) const
    {
        DynamicArray<BasicString> ret;
        SizeType start = 0;
        SizeType end;
        while ((end = FindStringHelper(static_cast<BasicStringView<CharType>>(*this), start, sep, cs)) != -1)
        {
            if (start != end || behavior == ESplitBehavior::KeepEmptyParts)
            {
                ret.Add(Slices(start, end - start));
            }
            start = end + sep.Length();
        }
        if (start != Length() || behavior == ESplitBehavior::KeepEmptyParts)
        {
            ret.Add(Slices(start, Length() - start));
        }
        return ret;
    }

    template <typename Elem, typename Traits, typename Alloc>
    DynamicArray<BasicString<Elem, Traits, Alloc>>
    BasicString<Elem, Traits, Alloc>::SplitAny(const BasicStringView<CharType>& sep, ESplitBehavior behavior,
                                               ECaseSensitivity cs) const
    {
        DynamicArray<BasicString> ret;
        SizeType start = 0;
        SizeType end;
        while ((end = FindAnyCharHelper(static_cast<BasicStringView<CharType>>(*this), start, sep, cs)) != -1)
        {
            if (start != end || behavior == ESplitBehavior::KeepEmptyParts)
            {
                ret.Add(Slices(start, end - start));
            }
            start = end + 1;
        }
        if (start != Length() || behavior == ESplitBehavior::KeepEmptyParts)
        {
            ret.Add(Slices(start, Length() - start));
        }
        return ret;
    }

    using String = BasicString<char>;
}
