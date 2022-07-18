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
    void CORE_API SetupLocale();

    void CORE_API ShutdownLocale();

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

        static constexpr inline bool IsHighSurrogate(char32_t ucs4) noexcept
        {
            return (ucs4 & 0xfffffc00) == 0xd800; // 0xd800 + up to 1023 (0x3ff)
        }

        static constexpr inline bool IsLowSurrogate(char32_t ucs4) noexcept
        {
            return (ucs4 & 0xfffffc00) == 0xdc00; // 0xdc00 + up to 1023 (0x3ff)
        }

        static constexpr inline char32_t SurrogateToUcs4(char16_t high, char16_t low) noexcept
        {
            return (char32_t(high)<<10) + low - 0x35fdc00;
        }

        static inline char32_t FoldCase(const char16_t* ch, const char16_t* start)
        {
            char32_t ucs4 = *ch;
            if (IsLowSurrogate(ucs4) && ch > start && IsHighSurrogate(*(ch - 1)))
            {
                ucs4 = SurrogateToUcs4(*(ch - 1), static_cast<char16_t >(ucs4));
            }
            return Unicode::FoldCase(ucs4);
        }

        static char32_t ToUCS4(const char16_t* ch, const char16_t* start, const char16_t* end);
    };

    struct CORE_API Utf8
    {
        static bool ToUnicode(char16_t* dest, int32 destCapacity, int32* pDestLength, const char* src, int32_t srcLength);

        static int32 FoldStr(char* dest, int32 destCapacity, const char* src, int32 srcLength);

        static char32_t ToUCS4(const char* ch, const char* start, const char* end) noexcept;
    };

    struct CORE_API Utf32
    {
        static bool ToUnicode(char16_t* dest, int32 destCapacity, int32* pDestLength, const char32_t* src, int32_t srcLength);

        static constexpr inline char32_t ToUCS4(const char32_t * ch, const char32_t* start, const char32_t* end) noexcept
        {
            return *ch;
        }
    };
}

