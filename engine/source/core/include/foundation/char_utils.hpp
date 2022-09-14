#pragma once

#include "global.hpp"
#include "foundation/type_traits.hpp"
#include "foundation/encoding.hpp"
#include "math/generic_math.hpp"

namespace Engine
{
    /**
     * @deprecated
     */
    class CharUtils
    {
    public:
        template <typename TChar>
        static constexpr TChar ToUpper(TChar character)
        {
            return (TChar)(uint32(character) - ((uint32(character) - 'a' < 26u) << 5));
        }

        template <typename TChar>
        static constexpr TChar ToLower(TChar character)
        {
            return (TChar)(uint32(character) + ((uint32(character) - 'A' < 26u) << 5));
        }

        template <typename T1, typename T2>
        static constexpr int32 Compare(const T1* lhs, const T2* rhs, size_t len) noexcept
        {
            if constexpr (std::is_same_v<T1, T2>)
            {
                if (lhs == rhs)
                {
                    return 0;
                }
            }

            auto left = reinterpret_cast<const typename std::make_unsigned<T1>::type*>(lhs);
            auto right = reinterpret_cast<const typename std::make_unsigned<T2>::type*>(rhs);
            for (; 0 < len; --len, ++left, ++right)
            {
                if (*left != *right)
                {
                    return *left < *right ? -1 : +1;
                }
            }

            return 0;
        }

        template <typename T1, typename T2>
        static constexpr int32 Compare(const T1* lhs, size_t lLen, const T2* rhs, size_t rLen) noexcept
        {
            int32 result = Compare(lhs, rhs, Math::Min(lLen, rLen));

            if (result != 0)
            {
                return result;
            }

            if (lLen > rLen)
            {
                return 1;
            }

            if (lLen < rLen)
            {
                return -1;
            }

            return 0;
        }

        template <typename T1, typename T2>
        static constexpr int32 Compare(const T1* lhs, const T2* rhs) noexcept
        {
            return Compare(lhs, Length(lhs), rhs, Length(rhs));
        }

        template <typename TChar>
        static constexpr strsize Length(const TChar* c) noexcept
        {
            // find length of null-terminated sequence
            strsize count = 0;
            while (*c != TChar())
            {
                ++count;
                ++c;
            }

            return count;
        }

        template <typename T1, typename T2>
        static constexpr int32 UCompareInsensitive(const T1* lhs, const T2* rhs, size_t len)
        {
            if constexpr (std::is_same_v<T1, T2>)
            {
                if (lhs == rhs)
                {
                    return 0;
                }
            }

            for (size_t idx = 0; idx < len; ++idx)
            {
                int32 diff = Unicode::FoldCase((int32)lhs[idx]) - Unicode::FoldCase((int32)rhs[idx]);
                if (diff)
                {
                    return diff;
                }
            }

            return 0;
        }

        template <typename T1, typename T2>
        static constexpr int32 UCompareInsensitive(const T1* lhs, size_t lLen, const T2* rhs, size_t rLen)
        {
            if constexpr (std::is_same_v<T1, T2>)
            {
                if (lhs == rhs)
                {
                    return 0;
                }
            }

            int32 result = UCompareInsensitive(lhs, rhs, Math::Min(lLen, rLen));
            if (result)
            {
                return result;
            }

            if (lLen > rLen)
            {
                return 1;
            }

            if (lLen < rLen)
            {
                return -1;
            }

            return 0;
        }
    };
}
