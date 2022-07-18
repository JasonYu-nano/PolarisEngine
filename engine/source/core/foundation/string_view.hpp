#pragma once

#include "global.hpp"
#include "foundation/char_traits.hpp"
#include "foundation/details/string_algorithm.hpp"

namespace Engine
{
    template <CharConcept T>
    struct BasicStringView
    {
        using CharType = T;

        BasicStringView() = default;

        BasicStringView(const CharType* str, strsize length)
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

        constexpr strsize Length() const { return Len; }

        constexpr strsize& Length() { return Len; }

        constexpr const CharType* Data() const { return Str; }

        constexpr bool IsNull() const { return !Str; }

        constexpr bool IsEmpty() const { return Length() <= 0; }

        constexpr void RemovePrefix(strsize num) noexcept
        {
            ENSURE(num >= 0 && num < Len);
            Len -= num;
            Str += num;
        }

        constexpr void RemoveSuffix(strsize num) noexcept
        {
            ENSURE(num >= 0 && num < Len);
            Len -= num;
        }

        constexpr BasicStringView Slices(strsize pos, strsize len) const noexcept
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
            const strsize len = rhs.Len;
            if (Len < len)
            {
                return false;
            }
            return Traits::Compare(Str, rhs.Str, len) == 0;
        }

        constexpr bool StartsWith(const CharType ch) const noexcept
        {
            if (IsNull() || IsEmpty())
            {
                return false;
            }
            return Traits::Equals(Str, ch) == 0;
        }

        constexpr bool EndsWith(const BasicStringView rhs) const noexcept
        {
            const strsize len = rhs.Len;
            if (Len < len)
            {
                return false;
            }
            return Traits::Compare(Str + (Len - len), rhs.Str, len) == 0;
        }

        constexpr bool EndsWith(const CharType ch) const noexcept
        {
            if (IsEmpty())
            {
                return false;
            }
            return Traits::Equals(Str + Len - 1, ch) == 0;
        }

        bool inline Contains(BasicStringView needle)
        {
            return Private::FindString<CharType>(Str, Len, 0, needle.Str, needle.Len) == 0;
        }

        int32 inline IndexOf(BasicStringView needle)
        {
            return Private::FindString<CharType>(Str, Len, 0, needle.Str, needle.Len);
        }

        DynamicArray<BasicStringView> Split(BasicStringView sep, ESplitBehavior behavior = ESplitBehavior::KeepEmptyParts)
        {
            DynamicArray<BasicStringView> ret;
            strsize start = 0;
            strsize end;
            strsize extra = 0;
            while ((end = Private::FindString<CharType>(Str, Len, start + extra, sep.Str, sep.Len)) != -1)
            {
                if (start != end || behavior == ESplitBehavior::KeepEmptyParts)
                {
                    ret.Add(Slices(start, end - start));
                }
                start = end + sep.Len;
            }
            if (start != Len || behavior == ESplitBehavior::KeepEmptyParts)
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
        using Traits = CharTraits<CharType>;
        strsize Len = 0;
        const CharType* Str = nullptr;
    };

    using TStringView = BasicStringView<char_t>;

    using StringView = BasicStringView<ansi>;

    using U16StringView = BasicStringView<char16_t>;

    using U32StringView = BasicStringView<char32_t>;
}