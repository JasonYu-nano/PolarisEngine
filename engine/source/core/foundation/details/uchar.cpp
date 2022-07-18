#include "foundation/uchar.hpp"
#include "foundation/encoding.hpp"

namespace Engine
{
    #define FLAG(x) (1 << (x))

    bool UChar::IsSpaceHelper(char32_t ucs4) noexcept
    {
        return Encoding::IsSpace(static_cast<int32>(ucs4));
    }

    bool UChar::IsLetterHelper(char32_t ucs4) noexcept
    {
        // TODO:
        return false;
    }

    bool UChar::IsNumberHelper(char32_t ucs4) noexcept
    {
        // TODO:
        return false;
    }

    bool UChar::IsLetterOrNumberHelper(char32_t ucs4) noexcept
    {
        // TODO:
        return false;
    }

    void UChar::ToLower()
    {
        UCS = Encoding::ToLower(UCS);
    }

    void UChar::ToUpper()
    {
        UCS = Encoding::ToUpper(UCS);
    }

    bool UChar::IsUpper(char32_t ucs4)
    {
        return Encoding::IsUpper(static_cast<int32>(ucs4));
    }

    bool UChar::IsLower(char32_t ucs4)
    {
        return Encoding::IsLower(static_cast<int32>(ucs4));
    }

    char32_t UChar::FoldCase(const char16_t* ch, const char16_t* start)
    {
        char32_t ucs4 = *ch;
        if (IsLowSurrogate(ucs4) && ch > start && IsHighSurrogate(*(ch - 1)))
        {
            ucs4 = SurrogateToUcs4(*(ch - 1), ucs4);
        }
        return Unicode::FoldCase(ucs4);
    }

    char32_t UChar::FoldCase(const UChar* ch, const UChar* start)
    {
        char32_t ucs4 = ch->Unicode();
        if (IsLowSurrogate(ucs4) && ch > start && IsHighSurrogate((ch - 1)->Unicode()))
        {
            ucs4 = SurrogateToUcs4(*(ch - 1), ucs4);
        }
        return Unicode::FoldCase(ucs4);
    }
}