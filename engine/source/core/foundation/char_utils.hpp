#pragma once

#include "predefine/platform.hpp"
#include "foundation/type_traits.hpp"

namespace Engine
{
    class CharUtils
    {
    public:
        template <CharType TChar>
        static TChar ToUpper(TChar character)
        {
            return (TChar)(uint32(character) - ((uint32(character) - 'a' < 26u) << 5));
        }

        template <CharType TChar>
        static TChar ToLower(TChar character)
        {
            return (TChar)(uint32(character) + ((uint32(character) - 'A' < 26u) << 5));
        }

        template <CharType TChar>
        static size_t StrLen(const TChar* str)
        {
            static_assert(false, "Un support type");
            return 0;
        }

        template <>
        static size_t StrLen(const ansi* str)
        {
            return strlen(str);
        }

        template <>
        static size_t StrLen(const wchar* str)
        {
            return wcslen(str);
        }

        template <CharType TChar>
        static int32 StrCmp(const TChar* str1, const TChar* str2)
        {
            static_assert(false, "Un support type");
            return 0;
        }

        template <>
        static int32 StrCmp(const ansi* str1, const ansi* str2)
        {
            return strcmp(str1, str2);
        }

        template <>
        static int32 StrCmp(const wchar* str1, const wchar* str2)
        {
            return wcscmp(str1, str2);
        }
    };
}
