#include "foundation/encoding.hpp"
#include "predefine/prerequisite.hpp"
#include "math/limit.hpp"
#include "unicode/ustring.h"
#include "unicode/uchar.h"

namespace Encoding
{
    bool IsUpper(int32 ch)
    {
        return u_isupper(ch);
    }

    bool IsLower(int32 ch)
    {
        return u_islower(ch);
    }

    int32 ToUpper(int32 ch)
    {
        return u_toupper(ch);
    }

    int32 ToLower(int32 ch)
    {
        return u_tolower(ch);
    }

    int32 FoldCase(int32 ch)
    {
        return u_foldCase(ch, U_FOLD_CASE_DEFAULT);
    }

    bool IsSpace(int32 ch)
    {
        return u_isspace(ch);
    }
}

namespace Engine
{
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

    bool Utf8::ToUnicode(char16_t* dest, int32 destCapacity, int32* pDestLength, const char* src, int32_t srcLength)
    {
        UErrorCode err = U_ZERO_ERROR;
        u_strFromUTF8(dest, destCapacity, pDestLength, src, srcLength, &err);
        return err != U_INVALID_CHAR_FOUND;
    }

    bool Utf32::ToUnicode(char16_t* dest, int32 destCapacity, int32* pDestLength, const char32_t* src, int32_t srcLength)
    {
        UErrorCode err = U_ZERO_ERROR;
        u_strFromUTF32(dest, destCapacity, pDestLength, reinterpret_cast<const int32*>(src), srcLength, &err);
        return err != U_INVALID_CHAR_FOUND;
    }
}
