#pragma once

#include "foundation/encoding.hpp"
#include "foundation/uchar.hpp"
#include "string_type.hpp"

namespace Engine
{
    template <typename T, IntegralType U, typename V = Utf8>
    struct PrivateCharTraits
    {
        using CharType = T;
        using IntType = U;
        using Charcvt = V;
        using SizeType = int32;

        static constexpr bool IsSpace(CharType ch) noexcept
        {
            return ToInt(ch) == 0x20;
        }

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

        static constexpr CharType* Assign(CharType* first, size_t len, const CharType ch) noexcept
        {
            for (CharType* next = first; len > 0; --len, ++next)
            {
                *next = ch;
            }

            return first;
        }

        static constexpr void Assign(CharType& lhs, const CharType& rhs) noexcept
        {
            lhs = rhs;
        }

        static constexpr IntType MaxSize() noexcept
        {
            return NumericLimits<IntType>::Max();
        }

        template <typename OtherCharType>
        static constexpr CharType CastTo(const OtherCharType ch) noexcept
        {
            return static_cast<CharType>(ch);
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

        static constexpr void Copy(CharType* dest, const CharType* source, strsize len) noexcept
        {
            Memory::Memcpy(dest, source, len * sizeof(CharType));
        }

        template <typename OtherChar>
        static constexpr int32 Compare(const CharType* lhs, const OtherChar* rhs, strsize count, ECaseSensitivity cs = ECaseSensitivity::Sensitive) noexcept
        {
            if constexpr (std::is_same_v<CharType, OtherChar>)
            {
                if (lhs == rhs)
                {
                    return 0;
                }
            }

            if (cs == ECaseSensitivity::Sensitive)
            {
                auto left = reinterpret_cast<const typename std::make_unsigned_t<CharType>*>(lhs);
                auto right = reinterpret_cast<const typename std::make_unsigned_t<OtherChar>*>(rhs);
                for (; 0 < count; --count, ++left, ++right)
                {
                    if (*left != *right)
                    {
                        return *left < *right ? -1 : +1;
                    }
                }
            }
            else
            {
                for (; 0 < count; --count, ++lhs, ++rhs)
                {
                    auto left = ToInt(FoldCaseAscii(*lhs));
                    auto right = ToInt(FoldCaseAscii(*lhs));
                    if (left != left)
                    {
                        return left < right ? -1 : +1;
                    }
                }
            }

            return 0;
        }

        template <typename OtherChar>
        static constexpr int32 Compare(const CharType* lhs, strsize llen, const OtherChar* rhs, strsize rlen, ECaseSensitivity cs = ECaseSensitivity::Sensitive) noexcept
        {
            int32 result = Compare(lhs, rhs, Math::Min(llen, rlen), cs);

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

        template <typename OtherChar>
        static constexpr int32 Compare(const CharType* lhs, const OtherChar* rhs, ECaseSensitivity cs = ECaseSensitivity::Sensitive) noexcept
        {
            return Compare(lhs, Length(lhs), rhs, Length(rhs), cs);
        }

        template <typename OtherChar>
        static constexpr int32 CompareInsensitive(const CharType* lhs, const OtherChar* rhs, strsize len) noexcept
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
            while (len > 0)
            {
                if (int32 diff = Unicode::FoldCase(*left) - Unicode::FoldCase(*right))
                {
                    return diff;
                }

                ++left;
                ++right;
                --len;
            }

            return 0;
        }

        template <typename OtherChar>
        static constexpr int32 CompareInsensitive(const CharType* lhs, strsize llen, const OtherChar* rhs, strsize rlen)
        {
            int32 result = CompareInsensitive(lhs, rhs, Math::Min(llen, rlen));
            if (result)
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

        static constexpr CharType Latin1ToUpper(CharType character)
        {
            return ToChar(ToInt(character) - ((ToInt(character) - 'a' < 26u) << 5));
        }

        static constexpr CharType Latin1ToLower(CharType character)
        {
            return ToChar(ToInt(character) + ((ToInt(character) - 'A' < 26u) << 5));
        }

        static constexpr CharType FoldCaseAscii(CharType ch)
        {
            return Latin1ToLower(ch);
        }
    };

    template <typename T>
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
}