#pragma once

#include "foundation/char_traits.hpp"
#include "foundation/details/string_algorithm.hpp"

namespace Engine
{
    template <typename T, typename Traits = CharTraits<T>>
    struct BasicStringView
    {
        using CharType = T;
        using SizeType = typename Traits::SizeType;

        BasicStringView() = default;

        BasicStringView(const CharType* str, SizeType length)
            : Len(length)
            , Str(str)
        {}

        BasicStringView(const CharType* str)
            : Len(Traits::Length(str))
            , Str(str)
        {}

        BasicStringView(const BasicStringView& other)
            : Len(other.Len)
            , Str(other.Str)
        {}

        BasicStringView& operator= (const CharType* str)
        {
            Str = str;
            Len = Traits::Length(str);
            return *this;
        }

        constexpr SizeType Length() const { return Len; }

        constexpr SizeType& Length() { return Len; }

        constexpr const CharType* Data() const { return Str; }

        constexpr bool Null() const { return !Str; }

        constexpr bool Empty() const { return Len <= 0; }

        constexpr void RemovePrefix(SizeType num) noexcept
        {
            ENSURE(num >= 0 && num < Len);
            Len -= num;
            Str += num;
        }

        constexpr void RemoveSuffix(SizeType num) noexcept
        {
            ENSURE(num >= 0 && num < Len);
            Len -= num;
        }

        constexpr BasicStringView Slices(SizeType pos, SizeType len) const noexcept
        {
            ENSURE(pos >= 0 && len >= 0 && pos + len <= Len);
            return BasicStringView(Str + pos, len);
        }

        constexpr int32 Compare(const BasicStringView other) const noexcept
        {
            return Traits::Compare(Str, Len, other.Str, other.Len);
        }

        constexpr bool StartsWith(const BasicStringView rhs) const noexcept
        {
            const SizeType len = rhs.Len;
            if (Len < len)
            {
                return false;
            }
            return Traits::Compare(Str, rhs.Str, len) == 0;
        }

        constexpr bool StartsWith(const CharType ch) const noexcept
        {
            if (Null() || Empty())
            {
                return false;
            }
            return Traits::Equals(Str, ch) == 0;
        }

        constexpr bool EndsWith(const BasicStringView rhs) const noexcept
        {
            const SizeType len = rhs.Len;
            if (Len < len)
            {
                return false;
            }
            return Traits::Compare(Str + (Len - len), rhs.Str, len) == 0;
        }

        constexpr bool EndsWith(const CharType ch) const noexcept
        {
            if (Empty())
            {
                return false;
            }
            return Traits::Equals(Str + Len - 1, ch) == 0;
        }

        bool inline Contains(BasicStringView needle)
        {
            return Private::FindString<CharType, Traits, SizeType>(Str, Len, 0, needle.Str, needle.Len) == 0;
        }

        int32 inline IndexOf(BasicStringView needle)
        {
            return Private::FindString<CharType, Traits, SizeType>(Str, Len, 0, needle.Str, needle.Len);
        }

        Array<BasicStringView> Split(BasicStringView sep, ESplitBehavior behavior = KeepEmptyParts)
        {
            Array<BasicStringView> ret;
            SizeType start = 0;
            SizeType end;
            SizeType extra = 0;
            while ((end = Private::FindString<CharType, Traits, SizeType>(Str, Len, start + extra, sep.Str, sep.Len)) != -1)
            {
                if (start != end || behavior == KeepEmptyParts)
                {
                    ret.Add(Slices(start, end - start));
                }
                start = end + sep.Len;
            }
            if (start != Len || behavior == KeepEmptyParts)
            {
                ret.Add(Slices(start, Len - start));
            }
            return ret;
        }

        friend bool operator== (const BasicStringView& lhs, const BasicStringView& rhs)
        {
            return lhs.Len == rhs.Len && CharTraits<CharType>::Compare(lhs.Str, rhs.Str, lhs.Len) == 0;
        }

    private:
        SizeType Len = 0;
        const CharType* Str = nullptr;
    };

    using StringView = BasicStringView<char>;

    using U16StringView = BasicStringView<char16_t>;

    using U32StringView = BasicStringView<char32_t>;
}