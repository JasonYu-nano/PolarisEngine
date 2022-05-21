#pragma once

#include "predefine/platform.hpp"
#include "foundation/char_utils.hpp"
#include "foundation/uchar.hpp"
#include "type_traits.hpp"
//#include "math/generic_math.hpp"

namespace Engine
{
    template <CharConcept T, IntegralType U>
    struct PrivateCharTraits
    {
        using CharType = T;
        using IntType = U;

        static constexpr strsize Length(const CharType* str) noexcept
        {
            strsize len = 0;
            while (*str != CharType())
            {
                ++len;
                ++str;
            }
            return len;
        }

        static constexpr const CharType* Find(const CharType* first, strsize len, const CharType& ch) noexcept
        {
            for (; 0 < len; --len, ++first)
            {
                if (*first == ch)
                {
                    return first;
                }
            }
            return nullptr;
        }

        static constexpr CharType* Assign(const CharType* first, size_t len, const CharType ch) noexcept
        {
            for (CharType* next = first; len > 0; --len, ++next)
            {
                *next = ch;
            }

            return first;
        }

        static constexpr IntType ToInt(const CharType ch) noexcept
        {
            return static_cast<IntType>(ch);
        }

        static constexpr CharType ToChar(const IntType i) noexcept
        {
            return static_cast<CharType>(i);
        }

        static constexpr bool Equals(const CharType lhs, const CharType rhs) noexcept
        {
            return lhs == rhs;
        }

        template <CharConcept OtherChar>
        static constexpr int32 Compare(const CharType* lhs, const OtherChar* rhs, strsize count) noexcept
        {
            if constexpr (std::is_same_v<CharType, OtherChar>)
            {
                if (lhs == rhs)
                {
                    return 0;
                }
            }

            auto left = reinterpret_cast<const typename std::make_unsigned_t<CharType>*>(lhs);
            auto right = reinterpret_cast<const typename std::make_unsigned_t<OtherChar>*>(rhs);
            for (; 0 < count; --count, ++left, ++right)
            {
                if (*left != *right)
                {
                    return *left < *right ? -1 : +1;
                }
            }

            return 0;
        }

        template <CharConcept OtherChar>
        static constexpr int32 Compare(const CharType* lhs, strsize llen, const OtherChar* rhs, strsize rlen) noexcept
        {
            int32 result = Compare(lhs, rhs, Math::Min(llen, rlen));

            if (result != 0)
            {
                return result;
            }

            if (llen > rlen)
            {
                return 1;
            }

            if (llen < rlen)
            {
                return -1;
            }

            return 0;
        }

        template <CharConcept OtherChar>
        static constexpr int32 Compare(const CharType* lhs, const OtherChar* rhs) noexcept
        {
            return Compare(lhs, Length(lhs), rhs, Length(rhs));
        }
    };

    template <CharConcept T>
    struct CharTraits : private PrivateCharTraits<T, strsize> {};

    template <>
    struct CharTraits<char> : PrivateCharTraits<char, int8> {};

    template <>
    struct CharTraits<char16_t> : PrivateCharTraits<char16_t, uint16> {};

    template <>
    struct CharTraits<char32_t> : PrivateCharTraits<char16_t, uint32> {};

    template <>
    struct CharTraits<wchar> : PrivateCharTraits<wchar, wcharsize> {};

    template <>
    struct CharTraits<UChar> : PrivateCharTraits<UChar, uint16> {};


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
            Len = CharUtils::Length(str);
        }

        constexpr strsize Length() const { return Len; }

        constexpr strsize& Length() { return Len; }

        constexpr const CharType* Data() const { return Str; }

        constexpr bool IsNull() const { return !Str; }

        constexpr bool IsEmpty() const { return Length() <= 0; }

        constexpr void RemovePrefix(strsize num) noexcept
        {
            //TODO: Check size
            Len -= num;
            Str += num;
        }

        constexpr void RemoveSuffix(strsize num) noexcept
        {
            //TODO: Check size
            Len -= num;
        }

        constexpr BasicStringView Substr(strsize offset, strsize num) noexcept
        {
            return BasicStringView(Str + offset, num);
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