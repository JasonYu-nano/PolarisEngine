#pragma once

#include "global.hpp"
#include "math/generic_math.hpp"
#include "memory/memory.hpp"
#include "foundation/encoding.hpp"
#include "foundation/dynamic_array.hpp"
#include "foundation/string_type.hpp"

#define REHASH(a) \
    if (slMinusOne < sizeof(std::size_t) * CHAR_BIT)  \
        hashHaystack -= Traits::ToInt(a) << slMinusOne; \
    hashHaystack <<= 1

#define STR_INLINE_BUFFER_SIZE 500

namespace Engine::Private
{

    template <typename CharType>
    void _BMInitSkipTable(const CharType* needle, strsize len, CharType* skipTable, ECaseSensitivity cs = ECaseSensitivity::Sensitive)
    {
        typedef CharTraits<CharType> Traits;

        strsize l = Math::Min(len, 255);
        Memory::Memset(skipTable, l, 256 * sizeof(CharType));
        needle += len - l;
        while (l--)
        {
            if (cs == ECaseSensitivity::Sensitive)
            {
                skipTable[Traits::ToInt(*needle) & 0xff] = l;
            }
            else
            {
                skipTable[Unicode::FoldCase(Traits::ToInt(*needle)) & 0xff] = l;
            }
            ++needle;
        }
    }

    template <typename CharType>
    strsize _BMFind(const CharType* haystack, strsize alen, strsize from, const CharType* needle, strsize blen, const CharType* skipTable, ECaseSensitivity cs = ECaseSensitivity::Sensitive)
    {
        typedef CharTraits<CharType> Traits;

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
        const strsize plMinusOne = blen - 1;

        const CharType* current = haystack + from + plMinusOne;
        const CharType* end = haystack + alen;

        if (cs == ECaseSensitivity::Sensitive)
        {
            while (current < end)
            {
                size_t skip = Traits::ToInt(skipTable[Traits::ToInt(*current) & 0xff]);
                if (!skip)
                {
                    // possible match
                    while (skip < blen)
                    {
                        if (*(current - skip) != needle[plMinusOne - skip])
                        {
                            break;
                        }
                        ++skip;
                    }
                    if (skip > plMinusOne) // we have a match
                    {
                        return (strsize)(current - haystack) - plMinusOne;
                    }

                    // in case we don't have a match we are a bit inefficient as we only skip by one
                    // when we have the non matching char in the string.
                    if (skipTable[Traits::ToInt(*(current - skip)) & 0xff] == blen)
                    {
                        skip = blen - skip;
                    }
                    else
                    {
                        skip = 1;
                    }
                }
                if (current > end - skip)
                {
                    break;
                }
                current += skip;
            }
        }
        else
        {
            while (current < end)
            {
                size_t skip = Traits::ToInt(skipTable[Unicode::FoldCase(Traits::ToInt(*current) & 0xff)]);
                if (!skip)
                {
                    // possible match
                    while (skip < blen)
                    {
                        if (Unicode::FoldCase(Traits::ToInt(*(current - skip))) != Unicode::FoldCase(Traits::ToInt(needle[plMinusOne - skip])))
                        {
                            break;
                        }
                        ++skip;
                    }
                    if (skip > plMinusOne) // we have a match
                    {
                        return (strsize)(current - haystack) - plMinusOne;
                    }

                    // in case we don't have a match we are a bit inefficient as we only skip by one
                    // when we have the non matching char in the string.
                    if (skipTable[Unicode::FoldCase(Traits::ToInt(*(current - skip))) & 0xff] == blen)
                    {
                        skip = blen - skip;
                    }
                    else
                    {
                        skip = 1;
                    }
                }
                if (current > end - skip)
                {
                    break;
                }
                current += skip;
            }
        }
        return -1;
    }

    template <typename CharType>
    strsize FindChar(const CharType* haystack, strsize len, strsize from, CharType ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive)
    {
        typedef CharTraits<CharType> Traits;

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
                if (cs == ECaseSensitivity::Sensitive)
                {
                    if (*current == ch)
                    {
                        return static_cast<strsize>(current - begin);
                    }
                }
                else
                {
                    if (Unicode::FoldCase(static_cast<char32_t>(*current)) == Unicode::FoldCase(static_cast<char32_t>(ch)))
                    {
                        return static_cast<strsize>(current - begin);
                    }
                }
                ++current;
            }
        }
        return -1;
    }

    template <typename CharType>
    strsize FindStringBoyerMoore(const CharType* haystack, strsize alen, strsize from, const CharType* needle, strsize blen, ECaseSensitivity cs = ECaseSensitivity::Sensitive)
    {
        CharType skipTable[256];
        _BMInitSkipTable<CharType>(needle, blen, skipTable, cs);
        return _BMFind<CharType>(haystack, alen, from, needle, blen, skipTable, cs);
    }

    template <typename CharType>
    strsize FindString(const CharType* haystack, strsize alen, strsize from, const CharType* needle, strsize blen, ECaseSensitivity cs = ECaseSensitivity::Sensitive)
    {
        typedef CharTraits<CharType> Traits;

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
            return from;
        }
        if (!alen)
        {
            return -1;
        }

        if (blen == 1)
        {
            return FindChar(haystack, alen, from, *needle, cs);
        }

        /*
            We use the Boyer-Moore algorithm in cases where the overhead
            for the skip table should pay off, otherwise we use a simple
            hash function.
        */
        if (alen > 500 && blen > 5)
        {
            return FindStringBoyerMoore<CharType>(haystack, alen, from, needle, blen, cs);
        }

        /*
            We use some hashing for efficiency's sake. Instead of
            comparing strings, we compare the hash value of str with that
            of a part of this string. Only if that matches, we call
            compare().
        */
        const CharType* current = haystack + from;
        const CharType* end = haystack + (alen - blen);
        const size_t slMinusOne = blen - 1;
        size_t hashNeedle = 0, hashHaystack = 0;
        strsize idx;

        for (idx = 0; idx < blen; ++idx)
        {
            hashNeedle = ((hashNeedle<<1) + Traits::ToInt(needle[idx]));
            hashHaystack = ((hashHaystack<<1) + Traits::ToInt(current[idx]));
        }
        hashHaystack -= Traits::ToInt(current[slMinusOne]);

        while (current <= end)
        {
            hashHaystack += Traits::ToInt(current[slMinusOne]);
            if (hashHaystack == hashNeedle && Traits::Compare(needle, current, blen) == 0)
            {
                return (strsize)(current - haystack);
            }

            REHASH(*current);
            ++current;
        }
        return -1;
    }

    template <typename CharType>
    static strsize FindLastChar(const CharType* haystack, strsize len, CharType ch, strsize from, ECaseSensitivity cs = ECaseSensitivity::Sensitive)
    {
        typedef CharTraits<CharType> Traits;

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
            if (cs == ECaseSensitivity::Sensitive)
            {
                for (; n >= haystack; --n)
                {
                    if (Traits::ToInt(*n) == c)
                    {
                        return static_cast<strsize>(n - haystack);
                    }
                }
            }
            else
            {
                c = Encoding::FoldCase(c);
                for (; n >= haystack; --n)
                {
                    if (Encoding::FoldCase(Traits::ToInt(*n)) == c)
                    {
                        return n - haystack;
                    }
                }
            }
        }
        return -1;
    }

    template <typename CharType>
    strsize FindLastString(const CharType* haystack, strsize alen, strsize from, const CharType* needle, strsize blen, ECaseSensitivity cs = ECaseSensitivity::Sensitive)
    {
        typedef CharTraits<CharType> Traits;

        if (blen == 1)
        {
            return FindLastChar<CharType>(haystack, alen, *needle, from, cs);
        }

        if (from < 0)
        {
            from += alen;
        }
        if (from == alen && blen == 0)
        {
            return from;
        }
        const strsize delta = alen - blen;
        if (from < 0 || from >= alen || delta < 0)
        {
            return -1;
        }
        if (from > delta)
        {
            from = delta;
        }

        const auto *end = haystack;
        haystack += from;
        const strsize slMinusOne = blen ? blen - 1 : 0;
        const auto *n = needle + slMinusOne;
        const auto *h = haystack + slMinusOne;
        size_t hashNeedle = 0, hashHaystack = 0;

        for (strsize idx = 0; idx < blen; ++idx)
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
                return haystack - end;
            }
            --haystack;
            REHASH(Traits::ToInt(haystack[blen]));
        }
        return -1;
    }

    void UBMInitSkipTableInsensitive(const char16_t* needle, strsize len, char16_t* skipTable);

    strsize UBMFindInsensitive(const char16_t* haystack, strsize alen, strsize from, const char16_t* needle, strsize blen, const char16_t* skipTable);

    strsize UFindStringInsensitive(const char16_t* haystack, strsize alen, strsize from, const char16_t* needle, strsize blen);

//    template <typename CharType, typename Allocator>
//    void SplitString(const CharType* source, strsize alen, const CharType* sep, strsize blen, DynamicArray<int32, Allocator>& outPos)
//    {
//        strsize start = 0;
//        strsize end;
//        strsize extra = 0;
//        while ((end = FindString(source, alen, start + extra, sep, blen)) != -1)
//        {
//            if (start != end || behavior == ESplitBehavior::KeepEmptyParts)
//            {
//                outPos.Add(Source.sliced(start, end - start));
//            }
//            start = end + sep.size();
//            extra = (sep.Length() == 0 ? 1 : 0);
//        }
//        if (start != Source.Length() || behavior == ESplitBehavior::KeepEmptyParts)
//            list.Add(source.sliced(start));
//        return list;
//    }
}