#pragma once

#include "foundation/dynamic_array.hpp"

namespace Encoding
{
    bool IsUpper(int32 ch);

    bool IsLower(int32 ch);

    int32 ToUpper(int32 ch);

    int32 ToLower(int32 ch);

    int32 FoldCase(int32 ch);

    bool IsSpace(int32 ch);
}

namespace Engine
{
    struct CORE_API Unicode
    {
        static bool IsUpper(char32_t ch);

        static bool IsLower(char32_t ch);

        static char32_t ToUpper(char32_t ch);

        static char32_t ToLower(char32_t ch);

        static char32_t FoldCase(char32_t ch);

        static bool IsSpace(char32_t ch);
    };

    struct CORE_API Utf16
    {
        static bool ToUtf8(char *dest, int32 destCapacity, int32* pDestLength, const char16_t* src, int32 srcLength);
    };

    struct CORE_API Utf8
    {
        static bool ToUnicode(char16_t* dest, int32 destCapacity, int32* pDestLength, const char* src, int32_t srcLength);
    };

    struct CORE_API Utf32
    {
        static bool ToUnicode(char16_t* dest, int32 destCapacity, int32* pDestLength, const char32_t* src, int32_t srcLength);
    };
}

