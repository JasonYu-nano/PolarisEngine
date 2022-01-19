#pragma once

#include "predefine/platform.hpp"
#include "foundation/type_traits.hpp"

namespace Engine
{
    class CharUtils
    {
    public:
        template <typename CharType, EnableIfT<IsCharV<CharType>, bool> = true>
        static CharType ToUpper(CharType character)
        {
            return (CharType)(uint32(character) - ((uint32(character) - 'a' < 26u) << 5));
        }

        template <typename CharType, EnableIfT<IsCharV<CharType>, bool> = true>
        static CharType ToLower(CharType character)
        {
            return (CharType)(uint32(character) + ((uint32(character) - 'A' < 26u) << 5));
        }

        template <class CharType>
        static size_t StrLen(const CharType* str)
        {
            return 0;
        }

        template <>
        static size_t StrLen(const schar* str)
        {
            return strlen(str);
        }

        template <>
        static size_t StrLen(const wchar* str)
        {
            return wcslen(str);
        }

        template <class CharType>
        static int32 StrCmp(const CharType* str1, const CharType* str2)
        {
            return 0;
        }

        template <>
        static int32 StrCmp(const schar* str1, const schar* str2)
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
