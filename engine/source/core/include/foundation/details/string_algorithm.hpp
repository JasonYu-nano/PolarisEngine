#pragma once

#include "global.hpp"
#include "math/generic_math.hpp"
#include "memory/memory.hpp"
#include "foundation/dynamic_array.hpp"
#include "foundation/string_type.hpp"
#include "foundation/char_traits.hpp"

#define REHASH(a) \
    if (slMinusOne < sizeof(std::size_t) * CHAR_BIT)  \
        hashHaystack -= Traits::ToInt(a ) << slMinusOne; \
    hashHaystack <<= 1

#define STR_INLINE_BUFFER_SIZE 500

namespace Engine::Private
{
    template <typename CharType, typename Traits, typename SizeType>
    void _BMInitSkipTable(const CharType* needle, SizeType len, int32* skipTable,
                            ECaseSensitivity cs = CaseSensitive)
    {
        SizeType l = Math::Min(len, 255);
        Memory::Memset(skipTable, -1, 256 * sizeof(int32));

        if (cs == CaseSensitive)
        {
            for (SizeType idx = len - l; idx < len; ++idx)
            {
                skipTable[Traits::ToInt(*(needle + idx)) & 0xff] = idx;
            }
        }
        else
        {
            for (SizeType idx = len - l; idx < len; ++idx)
            {
                skipTable[Traits::FoldCaseLatin1(Traits::ToInt(*(needle + idx))) & 0xff] = idx;
            }
        }
    }

    template <typename CharType, typename Traits, typename SizeType>
    SizeType _BMFind(const CharType* haystack, SizeType alen, SizeType from, const CharType* needle, SizeType blen,
                      const int32* skipTable, ECaseSensitivity cs = CaseSensitive)
    {
        if (from < 0)
        {
            from += alen;
        }

        if (from < 0 || from >= alen)
        {
            return -1;
        }

        if (blen == 0)
        {
            return from > alen ? -1 : from;
        }

        const CharType* start = haystack;

        if (cs == CaseSensitive)
        {
            int32 skip;

            for (SizeType idx = from; idx <= alen - blen; idx += skip)
            {
                skip = 0;
                for (SizeType needleIdx = blen - 1; needleIdx >= 0; --needleIdx)
                {
                    if (needle[needleIdx] != start[idx + needleIdx])
                    {
                        skip = needleIdx - skipTable[Traits::ToInt(start[idx + needleIdx]) & 0xff];
                        if (skip < 1)
                        {
                            skip = 1;
                        }
                        break;
                    }
                }

                if (skip == 0)
                {
                    return idx;
                }
            }
        }
        else
        {
            int32 skip;

            for (SizeType idx = from; idx <= alen - blen; idx += skip)
            {
                skip = 0;
                for (SizeType needleIdx = blen - 1; needleIdx >= 0; --needleIdx)
                {
                    if (Traits::FoldCaseLatin1(needle[needleIdx]) != Traits::FoldCaseLatin1(start[idx + needleIdx]))
                    {
                        skip = needleIdx - skipTable[Traits::ToInt(start[idx + needleIdx]) & 0xff];
                        if (skip < 1)
                        {
                            skip = 1;
                        }
                        break;
                    }
                }

                if (skip == 0)
                {
                    return idx;
                }
            }
        }
        return -1;
    }

    template <typename CharType, typename Traits, typename SizeType>
    SizeType FindChar(const CharType* haystack, SizeType len, SizeType from, CharType ch,
                     ECaseSensitivity cs = CaseSensitive)
    {
        if (from < 0)
        {
            from += len;
        }

        if (from >= 0 && from < len)
        {
            const CharType* begin = haystack;
            const CharType* current = begin + from;
            const CharType* end = haystack + len;
            while (current < end)
            {
                if (cs == CaseSensitive)
                {
                    if (*current == ch)
                    {
                        return static_cast<SizeType>(current - begin);
                    }
                }
                else
                {
                    if (Traits::FoldCaseLatin1(static_cast<char32_t>(*current)) ==
                        Traits::FoldCaseLatin1(static_cast<char32_t>(ch)))
                    {
                        return static_cast<SizeType>(current - begin);
                    }
                }
                ++current;
            }
        }
        return -1;
    }

    template <typename CharType, typename Traits, typename SizeType>
    SizeType FindStringBoyerMoore(const CharType* haystack, SizeType alen, SizeType from, const CharType* needle, SizeType blen,
                         ECaseSensitivity cs = CaseSensitive)
    {
        int32 skipTable[256];
        _BMInitSkipTable<CharType, Traits, SizeType>(needle, blen, skipTable, cs);
        return _BMFind<CharType, Traits, SizeType>(haystack, alen, from, needle, blen, skipTable, cs);
    }

    template <typename CharType, typename Traits, typename SizeType>
    SizeType FindStringHash(const CharType* haystack, SizeType alen, SizeType from, const CharType* needle, SizeType blen,
                       ECaseSensitivity cs = CaseSensitive)
    {
        const CharType* current = haystack + from;
        const CharType* end = haystack + (alen - blen);
        const size_t slMinusOne = blen - 1;
        size_t hashNeedle = 0, hashHaystack = 0;
        SizeType idx;

        if (cs == CaseSensitive)
        {
            for (idx = 0; idx < blen; ++idx)
            {
                hashNeedle = (hashNeedle << 1) + Traits::ToInt(needle[idx]);
                hashHaystack = (hashHaystack << 1) + Traits::ToInt(current[idx]);
            }
            hashHaystack -= Traits::ToInt(current[slMinusOne]);

            while (current <= end)
            {
                hashHaystack += Traits::ToInt(current[slMinusOne]);
                if (hashHaystack == hashNeedle && Traits::Compare(needle, current, blen) == 0)
                {
                    return (SizeType) (current - haystack);
                }

                REHASH(*current);
                ++current;
            }
        }
        else
        {
            for (idx = 0; idx < blen; ++idx)
            {
                hashNeedle = (hashNeedle << 1) + Traits::FoldCaseLatin1(Traits::ToInt(needle[idx]));
                hashHaystack = (hashHaystack << 1) + Traits::FoldCaseLatin1(Traits::ToInt(current[idx]));
            }
            hashHaystack -= Traits::FoldCaseLatin1(Traits::ToInt(current[slMinusOne]));

            while (current <= end)
            {
                hashHaystack += Traits::FoldCaseLatin1(Traits::ToInt(current[slMinusOne]));
                if (hashHaystack == hashNeedle && Traits::Compare(needle, current, blen, CaseInsensitive) == 0)
                {
                    return (SizeType) (current - haystack);
                }

                REHASH(Traits::CastTo(Traits::FoldCaseLatin1(Traits::ToInt(*current))));
                ++current;
            }
        }

        return -1;
    }

    template <typename CharType, typename Traits, typename SizeType>
    SizeType FindString(const CharType* haystack, SizeType alen, SizeType from, const CharType* needle, SizeType blen,
                       ECaseSensitivity cs = CaseSensitive)
    {
        if (from < 0)
        {
            from += alen;
        }
        if (blen + from > (alen))
        {
            return -1;
        }
        if (!blen)
        {
            return -1;
        }
        if (!alen)
        {
            return -1;
        }

        if (blen == 1)
        {
            return FindChar<CharType, Traits, SizeType>(haystack, alen, from, *needle, cs);
        }

        /*
            We use the Boyer-Moore algorithm in cases where the overhead
            for the skip table should pay off, otherwise we use a simple
            hash function.
        */
        if (alen > 500 && blen > 5)
        {
            return FindStringBoyerMoore<CharType, Traits, SizeType>(haystack, alen, from, needle, blen, cs);
        }

        /*
            We use some hashing for efficiency's sake. Instead of
            comparing strings, we compare the hash value of str with that
            of a part of this string. Only if that matches, we call
            compare().
        */
        return FindStringHash<CharType, Traits, SizeType>(haystack, alen, from, needle, blen, cs);
    }

    template <typename CharType, typename Traits, typename SizeType>
    static SizeType FindLastChar(const CharType* haystack, SizeType len, CharType ch, SizeType from,
                                ECaseSensitivity cs = CaseSensitive)
    {
        if (len == 0)
        {
            return -1;
        }
        if (from < 0)
        {
            from += len;
        }
        else if (from < 0 || from >= len)
        {
            return -1;
        }
        if (from >= 0)
        {
            auto c = Traits::ToInt(ch);
            auto n = haystack + from;
            if (cs == CaseSensitive)
            {
                for (; n >= haystack; --n)
                {
                    if (Traits::ToInt(*n) == c)
                    {
                        return static_cast<SizeType>(n - haystack);
                    }
                }
            }
            else
            {
                c = Traits::ToInt(Traits::FoldCaseLatin1(ch));
                for (; n >= haystack; --n)
                {
                    if (Traits::ToInt(Traits::FoldCaseLatin1(*n)) == c)
                    {
                        return static_cast<SizeType>(n - haystack);
                    }
                }
            }
        }
        return -1;
    }

    template <typename CharType, typename Traits, typename SizeType>
    SizeType FindLastString(const CharType* haystack, SizeType alen, SizeType from, const CharType* needle, SizeType blen,
                           ECaseSensitivity cs = CaseSensitive)
    {
        if (blen == 1)
        {
            return FindLastChar<CharType, Traits, SizeType>(haystack, alen, *needle, from, cs);
        }

        if (from < 0)
        {
            from += alen;
        }
        if (blen <= 0)
        {
            return -1;
        }
        const SizeType delta = alen - blen;
        if (from < 0 || from >= alen || delta < 0)
        {
            return -1;
        }
        if (from > delta)
        {
            from = delta;
        }

        const auto* end = haystack;
        haystack += from;
        const SizeType slMinusOne = blen ? blen - 1 : 0;
        const auto* n = needle + slMinusOne;
        const auto* h = haystack + slMinusOne;
        size_t hashNeedle = 0, hashHaystack = 0;

        if (cs == CaseSensitive)
        {
            for (SizeType idx = 0; idx < blen; ++idx)
            {
                hashNeedle = (hashNeedle << 1) + Traits::ToInt(*(n - idx));
                hashHaystack = (hashHaystack << 1) + Traits::ToInt(*(h - idx));
            }
            hashHaystack -= Traits::ToInt(*haystack);

            while (haystack >= end)
            {
                hashHaystack += Traits::ToInt(*haystack);
                if (hashHaystack == hashNeedle && Traits::Compare(needle, haystack, blen) == 0)
                {
                    return static_cast<SizeType>(haystack - end);
                }
                --haystack;
                REHASH(Traits::ToInt(haystack[blen]));
            }
        }
        else
        {
            for (SizeType idx = 0; idx < blen; ++idx)
            {
                hashNeedle = (hashNeedle << 1) + Traits::FoldCaseLatin1(*(n - idx));
                hashHaystack = (hashHaystack << 1) + Traits::FoldCaseLatin1(*(h - idx));
            }
            hashHaystack -= Traits::FoldCaseLatin1(*haystack);

            while (haystack >= end)
            {
                hashHaystack += Traits::FoldCaseLatin1(*haystack);
                if (hashHaystack == hashNeedle && Traits::Compare(needle, haystack, blen, CaseInsensitive) == 0)
                {
                    return static_cast<SizeType>(haystack - end);
                }
                --haystack;
                REHASH(Traits::FoldCaseLatin1(haystack[blen]));
            }
        }
        return -1;
    }

    template <typename CharType, typename Traits, typename SizeType>
    class StringMatcher
    {
    public:
        StringMatcher(const CharType* pattern, SizeType len, ECaseSensitivity cs)
                : CS(cs), Length(len), Pattern(pattern)
        {
            UpdateSkipTable();
        }

        SizeType IndexIn(const CharType* str, SizeType len, SizeType from = 0) const
        {
            from = from < 0 ? 0 : from;

            return _BMFind<CharType, Traits, SizeType>(str, len, from, Pattern, Length, SkipTable, CS);
        }

    private:
        void UpdateSkipTable()
        {
            _BMInitSkipTable<CharType, Traits, SizeType>(Pattern, Length, SkipTable, CS);
        }

        ECaseSensitivity CS;
        SizeType Length;
        const CharType* Pattern;
        int32 SkipTable[256] = {};
    };
}