#pragma once

#include "definitions_core.hpp"
#include "global.hpp"
#include "log/logger.hpp"

namespace Engine
{
    #define LAST_VALID_CHAR 0x10ffff

    class CORE_API UChar
    {
    public:

        enum SpecialCharacter
        {
            Null = 0x0000,
            Tabulation = 0x0009,
            LineFeed = 0x000a,
            FormFeed = 0x000c,
            CarriageReturn = 0x000d,
            Space = 0x0020,
            Nbsp = 0x00a0,
            SoftHyphen = 0x00ad,
            ReplacementCharacter = 0xfffd,
            ObjectReplacementCharacter = 0xfffc,
            ByteOrderMark = 0xfeff,
            ByteOrderSwapped = 0xfffe,
            ParagraphSeparator = 0x2029,
            LineSeparator = 0x2028,
            VisualTabCharacter = 0x2192,
            LastValidCodePoint = 0x10ffff
        };

        enum Category
        {
            Mark_NonSpacing,          //   Mn
            Mark_SpacingCombining,    //   Mc
            Mark_Enclosing,           //   Me

            Number_DecimalDigit,      //   Nd
            Number_Letter,            //   Nl
            Number_Other,             //   No

            Separator_Space,          //   Zs
            Separator_Line,           //   Zl
            Separator_Paragraph,      //   Zp

            Other_Control,            //   Cc
            Other_Format,             //   Cf
            Other_Surrogate,          //   Cs
            Other_PrivateUse,         //   Co
            Other_NotAssigned,        //   Cn

            Letter_Uppercase,         //   Lu
            Letter_Lowercase,         //   Ll
            Letter_Titlecase,         //   Lt
            Letter_Modifier,          //   Lm
            Letter_Other,             //   Lo

            Punctuation_Connector,    //   Pc
            Punctuation_Dash,         //   Pd
            Punctuation_Open,         //   Ps
            Punctuation_Close,        //   Pe
            Punctuation_InitialQuote, //   Pi
            Punctuation_FinalQuote,   //   Pf
            Punctuation_Other,        //   Po

            Symbol_Math,              //   Sm
            Symbol_Currency,          //   Sc
            Symbol_Modifier,          //   Sk
            Symbol_Other              //   So
        };

        constexpr UChar() noexcept : UCS(0) {}
        constexpr UChar(uint16 rc) noexcept : UCS(rc) {}
        constexpr UChar(uint8 c, uint8 r) noexcept : UCS(char16_t((r << 8) | c)) {}
        constexpr UChar(int8 rc) noexcept : UCS(char16_t(rc)) {}
        constexpr UChar(uint32 rc) noexcept
        {
            ENSURE(rc <= 0xffff);
            UCS = char16_t(rc);
        }
        constexpr UChar(int32 rc) noexcept : UCS(uint32(rc)) {}
        constexpr UChar(char16_t ch) noexcept : UCS(ch) {}
#if PLATFORM_WINDOWS
        constexpr UChar(wchar ch) noexcept : UCS(char16_t(ch)) {}
#endif

        // Always implicit -- allow for 'x' => UChar conversions
        constexpr UChar(char c) noexcept : UCS(c) {}

        static constexpr UChar FromUCS2(char16_t c) noexcept { return UChar{c}; }

        constexpr inline char ToLatin1() const noexcept { return UCS > 0xff ? '\0' : char(UCS); }
        constexpr inline char16_t Unicode() const noexcept { return UCS; }
        constexpr inline char16_t& Unicode() noexcept { return UCS; }

        constexpr inline bool IsNull() const noexcept { return UCS == 0; }
        constexpr inline bool IsSpace() const noexcept { return UChar::IsSpace(UCS); }
        constexpr inline bool IsLetter() const noexcept { return UChar::IsLetter(UCS); }
        constexpr inline bool IsNumber() const noexcept { return UChar::IsNumber(UCS); }
        constexpr inline bool IsLetterOrNumber() const noexcept { return UChar::IsLetterOrNumber(UCS); }
        constexpr inline bool IsNonCharacter() const noexcept { return UChar::IsNonCharacter(UCS); }
        constexpr inline bool IsHighSurrogate() const noexcept { return UChar::IsHighSurrogate(UCS); }
        constexpr inline bool IsLowSurrogate() const noexcept { return UChar::IsLowSurrogate(UCS); }
        constexpr inline bool IsSurrogate() const noexcept { return UChar::IsSurrogate(UCS); }
        inline bool IsUpper() const noexcept { return UChar::IsUpper(UCS); }
        inline bool IsLower() const noexcept { return UChar::IsLower(UCS); }

        void ToLower();

        void ToUpper();

        static constexpr inline bool IsNonCharacter(char32_t ucs4) noexcept
        {
            return ucs4 >= 0xfdd0 && (ucs4 <= 0xfdef || (ucs4 & 0xfffe) == 0xfffe);
        }
        static constexpr inline bool IsHighSurrogate(char32_t ucs4) noexcept
        {
            return (ucs4 & 0xfffffc00) == 0xd800; // 0xd800 + up to 1023 (0x3ff)
        }
        static constexpr inline bool IsLowSurrogate(char32_t ucs4) noexcept
        {
            return (ucs4 & 0xfffffc00) == 0xdc00; // 0xdc00 + up to 1023 (0x3ff)
        }
        static constexpr inline bool IsSurrogate(char32_t ucs4) noexcept
        {
            return (ucs4 - 0xd800u < 2048u);
        }

        static bool IsUpper(char32_t ucs4);

        static bool IsLower(char32_t ucs4);

        static constexpr inline bool RequiresSurrogates(char32_t ucs4) noexcept
        {
            return (ucs4 >= 0x10000);
        }

        static constexpr inline char32_t SurrogateToUcs4(char16_t high, char16_t low) noexcept
        {
            // 0x010000 through 0x10ffff, provided params are actual high, low surrogates.
            // 0x010000 + ((high - 0xd800) << 10) + (low - 0xdc00), optimized:
            return (char32_t(high)<<10) + low - 0x35fdc00;
        }

        static constexpr inline char32_t SurrogateToUcs4(UChar high, UChar low) noexcept
        {
            return SurrogateToUcs4(high.UCS, low.UCS);
        }

        static constexpr inline char16_t HighSurrogate(char32_t ucs4) noexcept
        {
            return char16_t((ucs4>>10) + 0xd7c0);
        }

        static constexpr inline char16_t LowSurrogate(char32_t ucs4) noexcept
        {
            return char16_t(ucs4%0x400 + 0xdc00);
        }

        static char32_t FoldCase(const char16_t* ch, const char16_t* start);

        static char32_t FoldCase(const UChar* ch, const UChar* start);

        static constexpr inline bool IsSpace(char32_t ucs4) noexcept
        {
            // note that [0x09..0x0d] + 0x85 are exceptional Cc-s and must be handled explicitly
            return ucs4 == 0x20 || (ucs4 <= 0x0d && ucs4 >= 0x09)
                   || (ucs4 > 127 && (ucs4 == 0x85 || ucs4 == 0xa0 || UChar::IsSpaceHelper(ucs4)));
        }

        static constexpr inline bool IsLetter(char32_t ucs4) noexcept
        {
            return (ucs4 >= 'A' && ucs4 <= 'z' && (ucs4 >= 'a' || ucs4 <= 'Z'))
                   || (ucs4 > 127 && UChar::IsLetterHelper(ucs4));
        }

        static constexpr inline bool IsNumber(char32_t ucs4) noexcept
        {
            return (ucs4 <= '9' && ucs4 >= '0') || (ucs4 > 127 && UChar::IsNumberHelper(ucs4));
        }

        static constexpr inline bool IsLetterOrNumber(char32_t ucs4) noexcept
        {
            return (ucs4 >= 'A' && ucs4 <= 'z' && (ucs4 >= 'a' || ucs4 <= 'Z'))
                   || (ucs4 >= '0' && ucs4 <= '9')
                   || (ucs4 > 127 && UChar::IsLetterOrNumberHelper(ucs4));
        }

        inline explicit operator char16_t() const
        {
            return UCS;
        }

        inline explicit operator uint16() const
        {
            return UCS;
        }

        inline explicit operator char32_t() const
        {
            return UCS;
        }

        inline explicit operator int32() const
        {
            return UCS;
        }

        inline explicit operator char() const
        {
            return static_cast<char>(UCS);
        }

        friend constexpr inline bool operator==(UChar c1, UChar c2) noexcept { return c1.UCS == c2.UCS; }
        friend constexpr inline bool operator< (UChar c1, UChar c2) noexcept { return c1.UCS <  c2.UCS; }

        friend constexpr inline bool operator!=(UChar c1, UChar c2) noexcept { return !operator==(c1, c2); }
        friend constexpr inline bool operator>=(UChar c1, UChar c2) noexcept { return !operator< (c1, c2); }
        friend constexpr inline bool operator> (UChar c1, UChar c2) noexcept { return operator< (c2, c1); }
        friend constexpr inline bool operator<=(UChar c1, UChar c2) noexcept { return !operator< (c2, c1); }

        friend constexpr inline bool operator==(UChar lhs, std::nullptr_t) noexcept { return lhs.IsNull(); }
        friend constexpr inline bool operator< (UChar,     std::nullptr_t) noexcept { return false; }
        friend constexpr inline bool operator==(std::nullptr_t, UChar rhs) noexcept { return rhs.IsNull(); }
        friend constexpr inline bool operator< (std::nullptr_t, UChar rhs) noexcept { return !rhs.IsNull(); }

        friend constexpr inline bool operator!=(UChar lhs, std::nullptr_t) noexcept { return !operator==(lhs, nullptr); }
        friend constexpr inline bool operator>=(UChar lhs, std::nullptr_t) noexcept { return !operator< (lhs, nullptr); }
        friend constexpr inline bool operator> (UChar lhs, std::nullptr_t) noexcept { return operator< (nullptr, lhs); }
        friend constexpr inline bool operator<=(UChar lhs, std::nullptr_t) noexcept { return !operator< (nullptr, lhs); }

        friend constexpr inline bool operator!=(std::nullptr_t, UChar rhs) noexcept { return !operator==(nullptr, rhs); }
        friend constexpr inline bool operator>=(std::nullptr_t, UChar rhs) noexcept { return !operator< (nullptr, rhs); }
        friend constexpr inline bool operator> (std::nullptr_t, UChar rhs) noexcept { return operator< (rhs, nullptr); }
        friend constexpr inline bool operator<=(std::nullptr_t, UChar rhs) noexcept { return !operator< (rhs, nullptr); }

    private:
        static bool IsSpaceHelper(char32_t ucs4) noexcept;
        static bool IsLetterHelper(char32_t ucs4) noexcept;
        static bool IsNumberHelper(char32_t ucs4) noexcept;
        static bool IsLetterOrNumberHelper(char32_t ucs4) noexcept;

        char16_t UCS;
    };

    #define UCHAR_TO_UTF16(str) reinterpret_cast<char16_t*>(str)
    #define K_UCHAR_TO_UTF16(str) reinterpret_cast<const char16_t*>(str)
    #define UTF16_TO_UCHAR(str) reinterpret_cast<UChar*>(str)
    #define K_UTF16_TO_UCHAR(str) reinterpret_cast<const UChar*>(str)

    template <typename Type>
    constexpr bool IsCharV = std::_Is_any_of_v<std::remove_cv_t<Type>, char, wchar_t, char8_t, char16_t, char32_t, UChar>;

    template <typename T>
    concept CharConcept = IsCharV<T>;
}

template <>
struct std::make_unsigned<Engine::UChar>
{
    using type = unsigned short;
};

template <>
struct std::make_signed<Engine::UChar>
{
    using type = short;
};