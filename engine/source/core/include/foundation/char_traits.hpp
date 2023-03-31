#pragma once

#include "string_type.hpp"
#include "foundation/type_traits.hpp"
#include "memory/memory.hpp"
#include "math/generic_math.hpp"

namespace Engine
{
    template <typename T, IntegralType U, typename V = int32>
    struct PrivateCharTraits
    {
        using CharType = T;
        using IntType = U;
        using SizeType = V;

        static constexpr bool IsSpace(CharType ch) noexcept
        {
            return ToInt(ch) == 0x20;
        }

        static constexpr SizeType Length(const CharType* str) noexcept
        {
            SizeType len = 0;
            while (*str != CharType())
            {
                ++len;
                ++str;
            }
            return len;
        }

        static constexpr const CharType* Find(const CharType* first, SizeType len, const CharType& ch) noexcept
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

        static constexpr void Copy(CharType* dest, const CharType* source, SizeType len) noexcept
        {
            Memory::Memcpy(dest, source, len * sizeof(CharType));
        }

        template <typename OtherChar>
        static constexpr int32 Compare(const CharType* lhs, const OtherChar* rhs, SizeType count, ECaseSensitivity cs = CaseSensitive) noexcept
        {
            if constexpr (std::is_same_v<CharType, OtherChar>)
            {
                if (lhs == rhs)
                {
                    return 0;
                }
            }

            if (cs == CaseSensitive)
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
                    auto left = ToInt(FoldCaseLatin1(*lhs));
                    auto right = ToInt(FoldCaseLatin1(*lhs));
                    if (left != left)
                    {
                        return left < right ? -1 : +1;
                    }
                }
            }

            return 0;
        }

        template <typename OtherChar>
        static constexpr int32 Compare(const CharType* lhs, SizeType llen, const OtherChar* rhs, SizeType rlen, ECaseSensitivity cs = CaseSensitive) noexcept
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
        static constexpr int32 Compare(const CharType* lhs, const OtherChar* rhs, ECaseSensitivity cs = CaseSensitive) noexcept
        {
            return Compare(lhs, Length(lhs), rhs, Length(rhs), cs);
        }

        static constexpr CharType ToUpperLatin1(CharType character)
        {
            return ToChar(ToInt(character) - ((ToInt(character) - 'a' < 26u) << 5));
        }

        static constexpr CharType ToLowerLatin1(CharType character)
        {
            return ToChar(ToInt(character) + ((ToInt(character) - 'A' < 26u) << 5));
        }

        static constexpr CharType FoldCaseLatin1(CharType ch)
        {
            return ToLowerLatin1(ch);
        }

        static constexpr IntType EndOfFile() noexcept
        {
            return static_cast<IntType>(EOF);
        }
    };

    template <typename T>
    struct CharTraits : private PrivateCharTraits<T, int32> {};

    template <>
    struct CharTraits<char> : PrivateCharTraits<char, int8> {};

    template <>
    struct CharTraits<char16_t> : PrivateCharTraits<char16_t, uint16> {};

    template <>
    struct CharTraits<char32_t> : PrivateCharTraits<char16_t, uint32> {};

    template <>
    struct CharTraits<wchar> : PrivateCharTraits<wchar, wcharsize> {};
}