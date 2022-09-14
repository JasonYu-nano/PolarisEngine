#include "foundation/encoding.hpp"
#include "math/limit.hpp"
#include "unicode/ustring.h"
#include "unicode/uchar.h"
#include "unicode/ucasemap.h"

namespace Engine
{
    UCaseMap* GCsm;

    bool Unicode::IsUpper(char32_t ch)
    {
        return u_isupper(static_cast<int32>(ch));
    }

    bool Unicode::IsLower(char32_t ch)
    {
        return u_islower(static_cast<int32>(ch));
    }

    char32_t Unicode::ToUpper(char32_t ch)
    {
        return u_toupper(static_cast<int32>(ch));
    }

    char32_t Unicode::ToLower(char32_t ch)
    {
        return u_tolower(static_cast<int32>(ch));
    }

    char32_t Unicode::FoldCase(char32_t ch)
    {
        return u_foldCase(static_cast<int32>(ch), U_FOLD_CASE_DEFAULT);
    }

    bool Unicode::IsSpace(char32_t ch)
    {
        return u_isspace(static_cast<int32>(ch));
    }

    bool Utf16::ToUtf8(char* dest, int32 destCapacity, int32* pDestLength, const char16_t* src, int32 srcLength)
    {
        UErrorCode err = U_ZERO_ERROR;
        u_strToUTF8(dest, destCapacity, pDestLength, src, srcLength, &err);
        return err != U_INVALID_CHAR_FOUND;
    }

    char32_t Utf16::ToUCS4(const char16_t* ch, const char16_t* start, const char16_t* end)
    {
        char32_t ucs4 = *ch;
        if (IsLowSurrogate(ucs4) && ch > start && IsHighSurrogate(*(ch - 1)))
        {
            ucs4 = SurrogateToUcs4(*(ch - 1), static_cast<char16_t >(ucs4));
        }
        else if (IsHighSurrogate(ucs4) && (ch + 1) < end && (IsLowSurrogate(*(ch + 1))))
        {
            ucs4 = SurrogateToUcs4(ucs4, static_cast<char16_t >(*(ch + 1)));
        }
        return ucs4;
    }

    bool Utf8::ToUnicode(char16_t* dest, int32 destCapacity, int32* pDestLength, const char* src, int32_t srcLength)
    {
        UErrorCode err = U_ZERO_ERROR;
        u_strFromUTF8(dest, destCapacity, pDestLength, src, srcLength, &err);
        return err != U_INVALID_CHAR_FOUND;
    }

    int32 Utf8::FoldStr(char* dest, int32 destCapacity, const char* src, int32 srcLength)
    {
        ENSURE(GCsm);
        UErrorCode err = U_ZERO_ERROR;
        return ucasemap_utf8FoldCase(GCsm, dest, destCapacity, src, srcLength, &err);
    }

    char32_t Utf8::ToUCS4(const char* ch, const char* start, const char* end) noexcept
    {
        int32 offset = -1;
        uint32 ucs4;

        while (ch >= start && offset < 0)
        {
            unsigned char uch = static_cast<unsigned char>(*ch);
            if (uch < 0x80u)
            {
                ucs4 = uch;
                offset = 0;
            }
            else if (uch < 0xc0u)
            {
                // 0x80-0xbf not first byte
                --ch;
            }
            else if (uch < 0xe0u)
            {
                ucs4 = static_cast<uint32>(uch & 0x1f);
                offset = 1;
            }
            else if (uch < 0xf0u)
            {
                ucs4 = static_cast<uint32>(uch & 0x0f);
                offset = 2;
            }
            else if (uch < 0xf8u)
            {
                ucs4 = static_cast<uint32>(uch & 0x07);
                offset = 3;
            }
        }

        if (offset < 0 || ch + offset >= end)
        {
            return 0;
        }

        for (++ch; 0 < offset; --offset, ++ch)
        {
            unsigned char uch = static_cast<unsigned char>(*ch);
            if (uch < 0x80u || 0xc0u <= uch)
            {
                return 0; // not continuation byte
            }
            else
            {
                ucs4 = ucs4 << 6 | (uch & 0x3f);
            }
        }

        return ucs4;
    }

    bool Utf32::ToUnicode(char16_t* dest, int32 destCapacity, int32* pDestLength, const char32_t* src, int32_t srcLength)
    {
        UErrorCode err = U_ZERO_ERROR;
        u_strFromUTF32(dest, destCapacity, pDestLength, reinterpret_cast<const int32*>(src), srcLength, &err);
        return err != U_INVALID_CHAR_FOUND;
    }

    void CORE_API SetupLocale()
    {
        UErrorCode err = U_ZERO_ERROR;
        GCsm = ucasemap_open(NULL, U_FOLD_CASE_DEFAULT, &err);
    }

    void CORE_API ShutdownLocale()
    {
        if (GCsm)
        {
            ucasemap_close(GCsm);
            GCsm = nullptr;
        }

    }
}
