#include "foundation/details/string_algorithm.hpp"
#include "foundation/char_utils.hpp"
#include "foundation/string_view.hpp"

namespace Engine::Private
{
//    strsize FindChar(UStringView target, strsize from, UChar ch, ECaseSensitivity cs)
//    {
//        strsize len = target.Length();
//        if (from < 0)
//        {
//            from += len;
//        }
//
//        if (from >= 0 && from < len)
//        {
//            const UChar* begin = target.Data();
//            const UChar* current = begin + from;
//            const UChar* end = target.Data() + target.Length();
//            while (current < end)
//            {
//                if (cs == ECaseSensitivity::Sensitive)
//                {
//                    if (*current == ch)
//                    {
//                        return static_cast<strsize>(current - begin);
//                    }
//                }
//                else
//                {
//                    if (Encoding::FoldCase(static_cast<int32>(*current)) == Encoding::FoldCase(static_cast<int32>(ch)))
//                    {
//                        return static_cast<strsize>(current - begin);
//                    }
//                }
//                ++current;
//            }
//        }
//        return -1;
//    }

//    strsize FindString(UStringView target, strsize from, UStringView sub, ECaseSensitivity cs)
//    {
//        const strsize l = target.Length();
//        const strsize sl = sub.Length();
//        if (from < 0)
//        {
//            from += l;
//        }
//        if (sl + from > (l))
//        {
//            return -1;
//        }
//        if (!sl)
//        {
//            return from;
//        }
//        if (!l)
//        {
//            return -1;
//        }
//
//        if (sl == 1)
//        {
//            return FindChar(target, from, *sub.Data(), cs);
//        }
//
//        /*
//            We use the Boyer-Moore algorithm in cases where the overhead
//            for the skip table should pay off, otherwise we use a simple
//            hash function.
//        */
//        if (l > 500 && sl > 5)
//        {
//            return FindStringBoyerMoore(target, from, sub, cs);
//        }
//
//        auto sv = [sl](const char16_t *v) { return U16StringView(v, sl); };
//        auto ToUtf16 = [](const UStringView& view)
//        {
//            return reinterpret_cast<const char16_t*>(view.Data());
//        };
//        /*
//            We use some hashing for efficiency's sake. Instead of
//            comparing strings, we compare the hash value of str with that
//            of a part of this QString. Only if that matches, we call
//            qt_string_compare().
//        */
//        const char16_t* needle = ToUtf16(sub);
//        const char16_t* haystack = ToUtf16(target) + from;
//        const char16_t* end = ToUtf16(target) + (l - sl);
//        const size_t sl_minus_1 = sl - 1;
//        size_t hashNeedle = 0, hashHaystack = 0;
//        strsize idx;
//
//        if (cs == ECaseSensitivity::Sensitive)
//        {
//            for (idx = 0; idx < sl; ++idx)
//            {
//                hashNeedle = ((hashNeedle<<1) + needle[idx]);
//                hashHaystack = ((hashHaystack<<1) + haystack[idx]);
//            }
//            hashHaystack -= haystack[sl_minus_1];
//
//            while (haystack <= end)
//            {
//                hashHaystack += haystack[sl_minus_1];
//                if (hashHaystack == hashNeedle && CharUtils::Compare(needle, haystack, sl) == 0)
//                {
//                    return (strsize)(haystack - ToUtf16(target));
//                }
//
//                REHASH(*haystack);
//                ++haystack;
//            }
//        }
//        else
//        {
//            const char16_t* haystackStart = K_UCHAR_TO_UTF16(target.Data());
//            for (idx = 0; idx < sl; ++idx)
//            {
//                hashNeedle = (hashNeedle<<1) + UChar::FoldCase(needle + idx, needle);
//                hashHaystack = (hashHaystack<<1) + UChar::FoldCase(haystack + idx, haystackStart);
//            }
//            hashHaystack -= UChar::FoldCase(haystack + sl_minus_1, haystackStart);
//
//            while (haystack <= end)
//            {
//                hashHaystack += UChar::FoldCase(haystack + sl_minus_1, haystackStart);
//                if (hashHaystack == hashNeedle && CharUtils::UCompareInsensitive(needle, haystack, sl) == 0)
//                {
//                    return (strsize)(haystack - ToUtf16(target));
//                }
//
//                REHASH(UChar::FoldCase(haystack, haystackStart));
//                ++haystack;
//            }
//        }
//        return -1;
//    }

//    strsize FindString(UStringView target, strsize from, StringView sub, ECaseSensitivity cs)
//    {
//        strsize sl = sub.Length();
//        const char* current = sub.Data();
//        DynamicArray<UChar> unicode(sl + 1);
//        ENSURE(unicode.Capacity() >= sl);
//
//        UChar* dest = unicode.Data();
//        strsize step = sl;
//        while (step--)
//        {
//            *dest++ = (UChar)*current++;
//        }
//        return FindString(target, from, UStringView(unicode.Data(), sl), cs);
//    }

//    static void BMInitSkipTable(UStringView needle, char16_t* skipTable, ECaseSensitivity cs)
//    {
//        const char16_t* uc = K_UCHAR_TO_UTF16(needle.Data());
//        const strsize len = needle.Length();
//        strsize l = Math::Min(len, 255);
//        Memory::Memset(skipTable, l, 256 * sizeof(char16_t));
//        uc += len - l;
//        if (cs == ECaseSensitivity::Sensitive)
//        {
//            while (l--)
//            {
//                skipTable[*uc & 0xff] = l;
//                ++uc;
//            }
//        }
//        else
//        {
//            const char16_t *start = uc;
//            while (l--)
//            {
//                skipTable[Encoding::FoldCase(*uc) & 0xff] = l;
//                ++uc;
//            }
//        }
//    }
//
//    static inline strsize BMFind(UStringView target, strsize from, UStringView sub, const char16_t* skipTable, ECaseSensitivity cs)
//    {
//        const char16_t *uc = K_UCHAR_TO_UTF16(target.Data());
//        const strsize l = target.Length();
//        const char16_t *puc = K_UCHAR_TO_UTF16(sub.Data());
//        const strsize pl = sub.Length();
//
//        if (from < 0)
//        {
//            from += l;
//        }
//
//        if (from < 0 || from >= l)
//        {
//            return -1;
//        }
//
//        if (pl == 0)
//        {
//            return from > l ? -1 : from;
//        }
//        const strsize pl_minus_one = pl - 1;
//
//        const char16_t* current = uc + from + pl_minus_one;
//        const char16_t* end = uc + l;
//        if (cs == ECaseSensitivity::Sensitive)
//        {
//            while (current < end)
//            {
//                size_t skip = skipTable[*current & 0xff];
//                if (!skip)
//                {
//                    // possible match
//                    while (skip < pl)
//                    {
//                        if (*(current - skip) != puc[pl_minus_one-skip])
//                        {
//                            break;
//                        }
//                        ++skip;
//                    }
//                    if (skip > pl_minus_one) // we have a match
//                    {
//                        return (strsize)(current - uc) - pl_minus_one;
//                    }
//
//                    // in case we don't have a match we are a bit inefficient as we only skip by one
//                    // when we have the non matching char in the string.
//                    if (skipTable[*(current - skip) & 0xff] == pl)
//                    {
//                        skip = pl - skip;
//                    }
//                    else
//                    {
//                        skip = 1;
//                    }
//                }
//                if (current > end - skip)
//                {
//                    break;
//                }
//                current += skip;
//            }
//        }
//        else
//        {
//            while (current < end)
//            {
//                size_t skip = skipTable[Encoding::FoldCase(*current) & 0xff];
//                if (!skip)
//                {
//                    // possible match
//                    while (skip < pl)
//                    {
//                        if (Encoding::FoldCase(*(current - skip)) != Encoding::FoldCase(*(puc + pl_minus_one - skip)))
//                        {
//                            break;
//                        }
//                        ++skip;
//                    }
//                    if (skip > pl_minus_one) // we have a match
//                    {
//                        return (strsize)(current - uc) - pl_minus_one;
//                    }
//                    // in case we don't have a match we are a bit inefficient as we only skip by one
//                    // when we have the non matching char in the string.
//                    if (skipTable[Encoding::FoldCase(*(current - skip)) & 0xff] == pl)
//                    {
//                        skip = pl - skip;
//                    }
//                    else
//                    {
//                        skip = 1;
//                    }
//                }
//                if (current > end - skip)
//                {
//                    break;
//                }
//                current += skip;
//            }
//        }
//        return -1;
//    }

//    strsize FindStringBoyerMoore(UStringView target, strsize from, UStringView sub, ECaseSensitivity cs)
//    {
//        char16_t skipTable[256];
//        BMInitSkipTable(sub, skipTable, cs);
//        return BMFind(target, from, sub, skipTable, cs);
//    }

//    DynamicArray<UString> SplitString(UStringView Source, UStringView sep, ESplitBehavior behavior, ECaseSensitivity cs)
//    {
//        DynamicArray<UString> list;
//        strsize start = 0;
//        strsize end;
//        strsize extra = 0;
//        while ((end = FindString(Source, start + extra, sep, cs)) != -1)
//        {
//            if (start != end || behavior == ESplitBehavior::KeepEmptyParts)
//                list.Add(Source.sliced(start, end - start));
//            start = end + sep.size();
//            extra = (sep.Length() == 0 ? 1 : 0);
//        }
//        if (start != Source.Length() || behavior == ESplitBehavior::KeepEmptyParts)
//            list.Add(source.sliced(start));
//        return list;
//    }
}
