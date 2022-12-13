#pragma once

#include "foundation/details/compressed_pair.hpp"
#include "foundation/char_traits.hpp"
#include "foundation/string_view.hpp"
#include "memory/allocator_policies.hpp"
#include "spdlog/pattern_formatter.h"

namespace Engine
{
    template <typename Elem, typename Traits>
    struct StringVal
    {
        using CharType = Elem;
        static constexpr int32 INLINE_SIZE = (16 / sizeof(CharType) < 1) ? 1 : (16 / sizeof(CharType));

        StringVal() {};

        ~StringVal() {};

        bool LargeStringEngaged() const { return INLINE_SIZE < MaxSize; }

        Elem* GetPtr() { return LargeStringEngaged() ? UB.Ptr : UB.Buffer; }

        const Elem* GetPtr() const { return LargeStringEngaged() ? UB.Ptr : UB.Buffer; }

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
        using ViewType = BasicStringView<CharType, Traits>;

    public:
        BasicString() = default;

        BasicString(CharType ch);

        BasicString(const CharType* str, SizeType len = -1);

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

        bool operator< (const BasicString& other) const;

        bool operator< (const CharType* other) const;

        bool operator> (const BasicString& other) const;

        bool operator> (const CharType* other) const;

        BasicString operator+ (const BasicString& other) const;

        BasicString operator+ (const CharType* other) const;

        void operator+= (const BasicString& other);

        void operator+= (const CharType* other);

        BasicString operator/ (const BasicString& other);

        BasicString operator/ (const CharType* other);

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

        explicit operator ViewType() const
        {
            return ViewType(Data(), Length());
        }

        CharType* Data()
        {
            return Pair.Second().GetPtr();
        }

        const CharType* Data() const
        {
            return Pair.Second().GetPtr();
        }

        SizeType Length() const
        {
            return Size() > 0 ? Size() - 1 : 0;
        }

        SizeType Capacity() const
        {
            return Pair.Second().MaxSize;
        }

        NODISCARD bool Empty() const
        {
            return Size() <= 1;
        }

        void Truncate(SizeType pos);

        BasicString Slices(SizeType pos, SizeType num) const;

        bool StartsWith(const ViewType& view, ECaseSensitivity cs = CaseSensitive) const;

        bool StartsWith(const BasicString& str, ECaseSensitivity cs = CaseSensitive) const
        {
            return StartsWith(static_cast<ViewType>(str), cs);
        }

        bool StartsWith(const CharType* str, ECaseSensitivity cs = CaseSensitive) const
        {
            return StartsWith(ViewType(str), cs);
        }

        bool StartsWith(CharType ch, ECaseSensitivity cs = CaseSensitive) const
        {
            return StartsWith(ViewType(std::addressof(ch), 1), cs);
        }

        bool EndsWith(const ViewType& view, ECaseSensitivity cs = CaseSensitive) const;

        bool EndsWith(const BasicString& str, ECaseSensitivity cs = CaseSensitive) const
        {
            return EndsWith(static_cast<ViewType>(str), cs);
        }

        bool EndsWith(const CharType* str, ECaseSensitivity cs = CaseSensitive) const
        {
            return EndsWith(ViewType(str), cs);
        }

        bool EndsWith(CharType ch, ECaseSensitivity cs = CaseSensitive) const
        {
            return EndsWith(ViewType(std::addressof(ch), 1), cs);
        }

        BasicString& Append(const ViewType& view);

        BasicString& Append(const BasicString& str)
        {
            return Append(static_cast<ViewType>(str));
        }

        BasicString& Append(const CharType* str)
        {
            return Append(ViewType(str));
        }

        BasicString& Append(CharType ch)
        {
            return Append(ViewType(std::addressof(ch), 1));
        }

        BasicString& Prepend(const ViewType& view)
        {
            return Insert(0, view);
        }

        BasicString& Prepend(const BasicString& str)
        {
            return Prepend(static_cast<ViewType>(str));
        }

        BasicString& Prepend(const CharType* str)
        {
            return Prepend(ViewType(str));
        }

        BasicString& Prepend(CharType ch)
        {
            return Prepend(ViewType(std::addressof(ch), 1));
        }

        BasicString& Insert(SizeType index, const ViewType& view);

        BasicString& Insert(SizeType index, const BasicString& str)
        {
            return Insert(index, static_cast<ViewType>(str));
        }

        BasicString& Insert(SizeType index, const CharType* str)
        {
            return Insert(index, ViewType(str));
        }

        BasicString& Insert(SizeType index, CharType ch)
        {
            return Insert(index, ViewType(std::addressof(ch), 1));
        }

        BasicString& Remove(SizeType pos, SizeType num);

        BasicString& Remove(const ViewType& view, ECaseSensitivity cs = CaseSensitive);

        BasicString& Remove(const BasicString& str, ECaseSensitivity cs = CaseSensitive)
        {
            return Remove(static_cast<ViewType>(str), cs);
        }

        BasicString& Remove(const CharType* str, ECaseSensitivity cs = CaseSensitive)
        {
            return Remove(ViewType(str), cs);
        }

        BasicString& Remove(CharType ch, ECaseSensitivity cs = CaseSensitive)
        {
            return Remove(ViewType(std::addressof(ch), 1), cs);
        }

        BasicString& Replace(const ViewType& before, const ViewType& after, ECaseSensitivity cs = CaseSensitive)
        {
            ReplaceHelper(*this, before, after, cs);
            return *this;
        }

        BasicString& Replace(const BasicString& before, const BasicString& after, ECaseSensitivity cs = CaseSensitive)
        {
            return Replace(static_cast<ViewType>(before), static_cast<ViewType>(after), cs);
        }

        BasicString& Replace(const CharType* before, const CharType* after, ECaseSensitivity cs = CaseSensitive)
        {
            return Replace(ViewType(before), ViewType(after), cs);
        }

        /**
         * Removes n characters from the end of the string.
         * If n is greater than or equal to Length(), the result is an empty string;
         * @param n
         */
        void Chop(SizeType n);

        BasicString Chopped(SizeType n);

        SizeType IndexOf(const ViewType& view, ECaseSensitivity cs = CaseSensitive) const
        {
            return FindStringHelper(static_cast<ViewType>(*this), 0, view, cs);
        }

        SizeType IndexOf(const CharType* str, ECaseSensitivity cs = CaseSensitive) const
        {
            return IndexOf(ViewType(str), cs);
        }

        SizeType IndexOf(const BasicString& str, ECaseSensitivity cs = CaseSensitive) const
        {
            return IndexOf(static_cast<ViewType>(str), cs);
        }

        SizeType IndexOf(CharType ch, ECaseSensitivity cs = CaseSensitive) const
        {
            return IndexOf(ViewType(std::addressof(ch), 1), cs);
        }

        SizeType IndexOfAny(const ViewType& view, ECaseSensitivity cs = CaseSensitive) const
        {
            return FindAnyCharHelper(static_cast<ViewType>(*this), 0, view, cs);
        }

        SizeType IndexOfAny(const BasicString& str, ECaseSensitivity cs = CaseSensitive) const
        {
            return IndexOfAny(static_cast<ViewType>(str), cs);
        }

        SizeType IndexOfAny(const CharType* str, ECaseSensitivity cs = CaseSensitive) const
        {
            return IndexOfAny(ViewType(str), cs);
        }

        SizeType LastIndexOf(const ViewType& view, ECaseSensitivity cs = CaseSensitive) const
        {
            return FindLastHelper(static_cast<ViewType>(*this), -1, view, cs);
        }

        SizeType LastIndexOf(const BasicString& str, ECaseSensitivity cs = CaseSensitive) const
        {
            return LastIndexOf(static_cast<ViewType>(str), cs);
        }

        SizeType LastIndexOf(const CharType* str, ECaseSensitivity cs = CaseSensitive) const
        {
            return LastIndexOf(ViewType(str), cs);
        }

        SizeType LastIndexOf(CharType ch, ECaseSensitivity cs = CaseSensitive) const
        {
            return LastIndexOf(ViewType(std::addressof(ch), 1), cs);
        }

        SizeType LastIndexOfAny(const ViewType& view, ECaseSensitivity cs = CaseSensitive) const
        {
            return FindLastAnyCharHelper(static_cast<ViewType>(*this), 0, view, cs);
        }

        SizeType LastIndexOfAny(const CharType* str, ECaseSensitivity cs = CaseSensitive) const
        {
            return LastIndexOfAny(ViewType(str), cs);
        }

        SizeType LastIndexOfAny(const BasicString& str, ECaseSensitivity cs = CaseSensitive) const
        {
            return LastIndexOfAny(static_cast<ViewType>(str), cs);
        }

        bool Contains(const ViewType& view, ECaseSensitivity cs = CaseSensitive) const
        {
            return FindStringHelper(static_cast<ViewType>(*this), 0, view, cs) >= 0;
        }

        bool Contains(const CharType* str, ECaseSensitivity cs = CaseSensitive) const
        {
            return Contains(ViewType(str), cs);
        }

        bool Contains(const BasicString& str, ECaseSensitivity cs = CaseSensitive) const
        {
            return Contains(static_cast<ViewType>(str), cs);
        }

        bool Contains(CharType ch, ECaseSensitivity cs = CaseSensitive) const
        {
            return Contains(ViewType(std::addressof(ch), 1), cs);
        }

        int32 Count(const ViewType& view, ECaseSensitivity cs = CaseSensitive) const;

        int32 Count(const BasicString& str, ECaseSensitivity cs = CaseSensitive) const
        {
            return Count(static_cast<ViewType>(str), cs);
        }

        int32 Count(const CharType* str, ECaseSensitivity cs = CaseSensitive) const
        {
            return Count(ViewType(str), cs);
        }

        int32 Count(CharType ch, ECaseSensitivity cs = CaseSensitive) const
        {
            return Count(ViewType(std::addressof(ch), 1), cs);
        }

        BasicString& Fill(CharType ch, int32 num = -1);

        BasicString	Repeated(int32 times) const;

        BasicString Trimmed() const;

        bool IsUpperLatin1() const;

        bool IsLowerLatin1() const;

        void ToUpperLatin1();

        void ToLowerLatin1();

        DynamicArray<BasicString> Split(const ViewType& sep, ESplitBehavior behavior = KeepEmptyParts, ECaseSensitivity cs = CaseSensitive) const;

        DynamicArray<BasicString> Split(const BasicString& sep, ESplitBehavior behavior = KeepEmptyParts, ECaseSensitivity cs = CaseSensitive) const
        {
            return Split(static_cast<ViewType>(sep), behavior, cs);
        }

        DynamicArray<BasicString> Split(const CharType* sep, ESplitBehavior behavior = KeepEmptyParts, ECaseSensitivity cs = CaseSensitive) const
        {
            return Split(ViewType(sep), behavior, cs);
        }

        DynamicArray<BasicString> Split(CharType sep, ESplitBehavior behavior = KeepEmptyParts, ECaseSensitivity cs = CaseSensitive) const
        {
            return Split(ViewType(std::addressof(sep), 1), behavior, cs);
        }

        DynamicArray<BasicString> SplitAny(const ViewType& sep, ESplitBehavior behavior = KeepEmptyParts, ECaseSensitivity cs = CaseSensitive) const;

        DynamicArray<BasicString> SplitAny(const BasicString& sep, ESplitBehavior behavior = KeepEmptyParts, ECaseSensitivity cs = CaseSensitive) const
        {
            return SplitAny(static_cast<ViewType>(sep), behavior, cs);
        }

        DynamicArray<BasicString> SplitAny(const CharType* sep, ESplitBehavior behavior = KeepEmptyParts, ECaseSensitivity cs = CaseSensitive) const
        {
            return SplitAny(ViewType(sep), behavior, cs);
        }

        DynamicArray<BasicString> SplitAny(CharType sep, ESplitBehavior behavior = KeepEmptyParts, ECaseSensitivity cs = CaseSensitive) const
        {
            return SplitAny(ViewType(std::addressof(sep), 1), behavior, cs);
        }

        void Clear()
        {
            Truncate(0);
        }

        uint32 GetHashCode() const;

        template <typename... Args, std::enable_if_t<sizeof(Elem) == sizeof(char), bool> = true>
        static BasicString Format(const CharType* fmt, Args&&... args)
        {
            fmt::basic_memory_buffer<CharType> buffer;
            const auto& vargs = fmt::make_args_checked<Args...>(fmt, args...);
            fmt::detail::vformat_to(buffer, fmt::to_string_view(fmt), vargs);
            return BasicString(buffer.data(), static_cast<SizeType>(buffer.size()));
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

        void CopyAssign(const ViewType& right);

        static SizeType FindStringHelper(const ViewType& haystack, SizeType from, const ViewType& needle, ECaseSensitivity cs);

        static SizeType FindAnyCharHelper(const ViewType& haystack, SizeType from, const ViewType& needle, ECaseSensitivity cs);

        static SizeType FindLastHelper(const ViewType& haystack, SizeType from, const ViewType& needle, ECaseSensitivity cs);

        static SizeType FindLastAnyCharHelper(const ViewType& haystack, SizeType from, const ViewType& needle, ECaseSensitivity cs);

        static void ReplaceHelper(BasicString& source, const ViewType& before, const ViewType& after, ECaseSensitivity cs = CaseSensitive);

    private:
        SizeType AddUnconstructElement(SizeType count);

        void InsertUnconstructElement(SizeType index, SizeType count);

        void DestructElements(CharType* elems, SizeType count);

        void Expansion(SizeType destSize);

        SizeType CalculateGrowth(const SizeType newSize) const;

        void BecomeLarge(SizeType capacity);

        int32 Compare(const BasicString& other, ECaseSensitivity cs = CaseSensitive) const
        {
            return CharTraits::Compare(Data(), Length(), other.Data(), other.Length(), cs);
        }

        int32 Compare(const ViewType& other, ECaseSensitivity cs = CaseSensitive) const
        {
            return CharTraits::Compare(Data(), Length(), other.Data(), other.Length(), cs);
        }

    private:
        static constexpr auto INLINE_SIZE = StringVal<CharType, CharTraits>::INLINE_SIZE;
        static constexpr bool CAN_MEMORY_COPY = std::is_trivial_v<CharType>;

        CompressedPair<AllocatorType, StringVal<CharType, CharTraits>> Pair;
    };

    using String = BasicString<char>;
}

template<>
struct CORE_API std::hash<String>
{
    size_t operator()(const String& str) const
    {
        return str.GetHashCode();
    }
};

template<>
struct CORE_API std::less<String>
{
    bool operator()(const String& lhs, const String& rhs) const
    {
        return lhs < rhs;
    }
};

template <>
struct CORE_API fmt::formatter<String> : fmt::formatter<int>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const String& str, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "{0}", str.Data());
    }
};

#include "foundation/details/string_impl.hpp"