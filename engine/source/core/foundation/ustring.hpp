#pragma once

#include <string>
#include "foundation/uchar.hpp"
#include "foundation/string_view.hpp"
#include "foundation/char_utils.hpp"
#include "foundation/dynamic_array.hpp"
#include "foundation/string_type.hpp"

namespace Engine
{
    using UStringView = BasicStringView<UChar>;

    class CORE_API UString
    {
        static_assert(std::is_signed_v<strsize>, "UString don't support unsigned type");
    public:
        using SourceType = DynamicArray<UChar, InlineAllocator<8>>;
        using Iterator = SourceType::Iterator;
        using ConstIterator = SourceType::ConstIterator;

    public:
        UString() = default;

        UString(UChar ch);

        UString(const char* utf8, strsize len = -1);

        UString(const UChar* unicode, strsize len = -1);

        UString(strsize count, UChar c);

        UString(const UString& other) noexcept = default;

        UString(UString&& other) noexcept = default;

        ~UString() = default;

        UString& operator= (const UString& other) = default;

        UString& operator= (UString&& other) = default;

        UString& operator= (const char* utf8) { return *this = FromUtf8(StringView(utf8)); }

        UString& operator= (int utf8) { return *this; }

        UChar operator[] (strsize idx) const { ENSURE(idx < Length()); return Source[idx]; }

        UChar& operator[] (strsize idx) { ENSURE(idx < Length()); return Source[idx]; }

        explicit operator UStringView() const { return UStringView(Data(), Length()); }

        UChar At(strsize idx) const { ENSURE(idx < Length()); return Source[idx]; }

        /**
         * Returns a UString initialized with the first size characters of the Latin-1 string str.
         * @param StringView
         * @return
         */
        static UString FromLatin1(StringView str);

        /**
         * Returns a UString initialized with the first size bytes of the UTF-8 string str.
         * @param view
         * @return
         */

        static UString FromUtf8(StringView view);

        static UString FromUtf16(const char16_t* unicode, strsize len = -1);

        static UString FromUtf32(const char32_t* ucs4, strsize len = -1);

        static UString FromStdString(const std::string& str);

        DynamicArray<char> ToLatin1() const;

        DynamicArray<char> ToUtf8() const;

        std::string ToStdString() const;

        DynamicArray<wchar_t> ToWCharArray() const
        {
            DynamicArray<wchar_t> ret;
            if constexpr (sizeof(wchar_t ) == sizeof(UChar))
            {
                ret.Add(reinterpret_cast<const wchar_t*>(Data()), Length() + 1);
            }
            else
            {
                DynamicArray<char32_t> ucs4 = ToUCS4();
                ret.Add(reinterpret_cast<const wchar_t*>(ucs4.Data()), ucs4.Size());
            }

            return ret;
        }

        DynamicArray<char32_t> ToUCS4() const;

        strsize Length() const;

        strsize Capacity() const;

        inline const UChar* Data() const;

        inline UChar* Data();

        inline const char16_t* Utf16() const;

        bool IsNull() const;

        bool IsEmpty() const;

        void Resize(strsize len);

        void Truncate(strsize pos);

        UString Slices(strsize pos, strsize num) const;

        bool StartsWith(UStringView latin1, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        bool StartsWith(const char* latin1, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline bool StartsWith(const UString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline bool StartsWith(UChar ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        bool EndsWith(UStringView latin1, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        bool EndsWith(const char* latin1, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline bool EndsWith(const UString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline bool EndsWith(UChar ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        inline UString& Append(const char* latin1);
        inline UString& Append(UChar ch);
        inline UString& Append(const UString& str);
        UString& Append(const UChar* str, strsize len = -1);

        inline UString& Prepend(const char* latin1);
        inline UString& Prepend(const UChar* str, strsize len = -1);
        inline UString& Prepend(const UString& str);
        inline UString& Prepend(const UStringView& str);

        UString& Insert(strsize pos, const UChar* str, strsize len);

        UString& Remove(strsize pos, strsize num);
        UString& Remove(const char* latin1, ECaseSensitivity cs = ECaseSensitivity::Sensitive);
        UString& Remove(const UString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive);
        UString& Remove(UChar ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive);

        UString& Replace(const UString& before, const UString& after, ECaseSensitivity cs = ECaseSensitivity::Sensitive);

        bool IsUpper() const;
        bool IsLower() const;
        void ToUpper();
        void ToLower();

        inline void Clear();

        /**
         * Removes n characters from the end of the string.
         * If n is greater than or equal to Length(), the result is an empty string;
         * @param n
         */
        void Chop(strsize n);

        UString Chopped(strsize n);

        inline strsize IndexOf(UStringView str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline strsize IndexOf(const char* latin1, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline strsize IndexOf(const UString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline strsize IndexOf(UChar ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        inline strsize IndexOfAny(UStringView str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline strsize IndexOfAny(const char* latin1, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline strsize IndexOfAny(const UString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        inline strsize LastIndexOf(UStringView str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline strsize LastIndexOf(const UString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline strsize LastIndexOf(const char* latin1, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline strsize LastIndexOf(UChar ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        inline strsize LastIndexOfAny(UStringView str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline strsize LastIndexOfAny(const char* latin1, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline strsize LastIndexOfAny(const UString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        inline bool Contains(UStringView str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline bool Contains(const char* latin1, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline bool Contains(const UString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        inline bool Contains(UChar ch, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        int32 Count(const UString& str, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;
        int32 Count(const char* latin1, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        UString& Fill(UChar ch, int32 num = -1);

        UString	Repeated(int32 times) const;

        UString Trimmed() const;

        UString Simplified() const;

        DynamicArray<UString> Split(const UString& sep, ESplitBehavior behavior = ESplitBehavior::KeepEmptyParts, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        DynamicArray<UString> SplitAny(const UString& sep, ESplitBehavior behavior = ESplitBehavior::KeepEmptyParts, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        int32 Compare(UStringView other, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        int32 Compare(const char* other, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        inline int32 Compare(const UString& other, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        inline int32 Compare(UChar other, ECaseSensitivity cs = ECaseSensitivity::Sensitive) const;

        inline bool operator== (const UString& other) const
        {
            return CharUtils::Compare(Data(), Length(), other.Data(), other.Length()) == 0;
        }

        inline bool operator== (const UStringView& other) const
        {
            return CharUtils::Compare(Data(), Length(), other.Data(), other.Length()) == 0;
        }

        inline bool operator== (const char* other) const
        {
            return CharUtils::Compare(Data(), Length(), other, CharUtils::Length(other)) == 0;
        }

        inline Iterator begin();
        inline ConstIterator begin() const;
        inline Iterator end();
        inline ConstIterator end() const;

        inline Iterator rbegin();
        inline ConstIterator rbegin() const;
        inline Iterator rend();
        inline ConstIterator rend() const;

        inline ConstIterator cbegin();
        inline ConstIterator cend();
        inline ConstIterator crbegin();
        inline ConstIterator crend();

        template <typename... Args>
        static UString Formats(const char* fmt, Args&&... args)
        {
            fmt::basic_memory_buffer<char> buffer;
            const auto& vargs = fmt::make_args_checked<Args...>(fmt, args...);
            fmt::detail::vformat_to(buffer, fmt::to_string_view(fmt), vargs);
            return UString::FromUtf8(StringView(buffer.data(), static_cast<strsize>(buffer.size())));
        }

    protected:
        static strsize FindStringHelper(UStringView haystack, strsize from, UStringView needle, ECaseSensitivity cs);

        static strsize FindStringHelper(UStringView haystack, strsize from, StringView needle, ECaseSensitivity cs);

        static strsize FindAnyCharHelper(UStringView haystack, strsize from, UStringView needle, ECaseSensitivity cs);

        static strsize FindAnyCharHelper(UStringView haystack, strsize from, StringView needle, ECaseSensitivity cs);

        static strsize FindLastHelper(UStringView haystack, strsize from, UStringView needle, ECaseSensitivity cs);

        static strsize FindLastHelper(UStringView haystack, strsize from, StringView needle, ECaseSensitivity cs);

        static strsize FindLastAnyCharHelper(UStringView haystack, strsize from, UStringView needle, ECaseSensitivity cs);

        static strsize FindLastAnyCharHelper(UStringView haystack, strsize from, StringView needle, ECaseSensitivity cs);

        static void ReplaceHelper(UString& source, UStringView before, UStringView after, ECaseSensitivity cs = ECaseSensitivity::Sensitive);

        static void ReplaceHelper(UString& source, strsize* indices, int32 nIndices, strsize blen, const UChar*after, strsize alen);

        SourceType Source;
    };

    const UChar* UString::Data() const
    {
        return Source.Data();
    }

    UChar* UString::Data()
    {
        return Source.Data();
    }

    const char16_t* UString::Utf16() const
    {
        return reinterpret_cast<const char16_t*>(Data());
    }

    UString& UString::Append(const char* latin1)
    {
        return Append(UString::FromUtf8(latin1));
    }

    UString& UString::Append(UChar ch)
    {
        return Append(&ch, 1);
    }

    UString& UString::Append(const UString& str)
    {
        return Append(str.Data(), str.Length());
    }

    UString& UString::Prepend(const char* latin1)
    {
        return Prepend(UString::FromUtf8(latin1));
    }

    UString& UString::Prepend(const UChar* str, strsize len)
    {
        len = len >= 0 ? len : CharUtils::Length(str);
        return Prepend(UStringView(str, len));
    }

    UString& UString::Prepend(const UString& str)
    {
        return Prepend((UStringView)str);
    }

    UString& UString::Prepend(const UStringView& str)
    {
        return Insert(0, str.Data(), str.Length());
    }

    bool UString::StartsWith(const UString& str, ECaseSensitivity cs) const
    {
        return StartsWith((UStringView)str, cs);
    }

    bool UString::StartsWith(UChar ch, ECaseSensitivity cs) const
    {
        return StartsWith(UStringView(&ch, 1), cs);
    }

    bool UString::EndsWith(const UString& str, ECaseSensitivity cs) const
    {
        return EndsWith((UStringView)str, cs);
    }

    bool UString::EndsWith(UChar ch, ECaseSensitivity cs) const
    {
        return EndsWith(UStringView(&ch, 1), cs);
    }

    void UString::Clear()
    {
        Source.Clear();
    }

    strsize UString::IndexOf(UStringView str, ECaseSensitivity cs) const
    {
        return FindStringHelper((UStringView)*this, 0, str, cs);
    }

    strsize UString::IndexOf(const char* latin1, ECaseSensitivity cs) const
    {
        return FindStringHelper((UStringView)*this, 0 , latin1, cs);
    }

    strsize UString::IndexOf(const UString& str, ECaseSensitivity cs) const
    {
        return IndexOf((UStringView)str, cs);
    }

    strsize UString::IndexOf(UChar ch, ECaseSensitivity cs) const
    {
        return IndexOf(UStringView(&ch, 1), cs);
    }

    strsize UString::IndexOfAny(UStringView str, ECaseSensitivity cs) const
    {
        return FindAnyCharHelper((UStringView)*this, 0, str, cs);
    }

    strsize UString::IndexOfAny(const char* latin1, ECaseSensitivity cs) const
    {
        return FindAnyCharHelper((UStringView)*this, 0, latin1, cs);
    }

    strsize UString::IndexOfAny(const UString& str, ECaseSensitivity cs) const
    {
        return IndexOfAny(UStringView(str), cs);
    }

    strsize UString::LastIndexOf(UStringView str, ECaseSensitivity cs) const
    {
        return FindLastHelper((UStringView)*this, -1 , str, cs);
    }

    strsize UString::LastIndexOf(const UString& str, ECaseSensitivity cs) const
    {
        return LastIndexOf((UStringView)str, cs);
    }

    strsize UString::LastIndexOf(const char* latin1, ECaseSensitivity cs) const
    {
        return FindLastHelper((UStringView)*this, -1, StringView(latin1), cs);
    }

    strsize UString::LastIndexOf(UChar ch, ECaseSensitivity cs) const
    {
        return LastIndexOf(UStringView(&ch, 1), cs);
    }

    strsize UString::LastIndexOfAny(UStringView str, ECaseSensitivity cs) const
    {
        return FindLastAnyCharHelper((UStringView)*this, 0, str, cs);
    }

    strsize UString::LastIndexOfAny(const char* latin1, ECaseSensitivity cs) const
    {
        return FindLastAnyCharHelper((UStringView)*this, 0, latin1, cs);
    }

    strsize UString::LastIndexOfAny(const UString& str, ECaseSensitivity cs) const
    {
        return LastIndexOf(UStringView(str), cs);
    }

    bool UString::Contains(UStringView str, ECaseSensitivity cs) const
    {
        return FindStringHelper((UStringView)*this, 0, str, cs) >= 0;
    }

    bool UString::Contains(const char* latin1, ECaseSensitivity cs) const
    {
        return FindStringHelper((UStringView)*this, 0, latin1, cs) >= 0;
    }

    bool UString::Contains(const UString& str, ECaseSensitivity cs) const
    {
        return Contains((UStringView)str, cs);
    }

    bool UString::Contains(UChar ch, ECaseSensitivity cs) const
    {
        return Contains(UStringView(&ch, 1), cs);
    }

    UString::Iterator UString::begin()
    {
        return Source.begin();
    }

    UString::ConstIterator UString::begin() const
    {
        return Source.begin();
    }

    UString::Iterator UString::end()
    {
        return Iterator(Source, Length());
    }

    UString::ConstIterator UString::end() const
    {
        return ConstIterator(Source, Length());
    }

    UString::Iterator UString::rbegin()
    {
        return Iterator(Source, Length());
    }

    UString::ConstIterator UString::rbegin() const
    {
        return ConstIterator(Source, Length());
    }

    UString::Iterator UString::rend()
    {
        return Source.rend();
    }

    UString::ConstIterator UString::rend() const
    {
        return Source.rend();
    }

    UString::ConstIterator UString::cbegin()
    {
        return Source.cbegin();
    }

    UString::ConstIterator UString::cend()
    {
        return ConstIterator(Source, Length());
    }

    UString::ConstIterator UString::crbegin()
    {
        return ConstIterator(Source, Length());
    }

    UString::ConstIterator UString::crend()
    {
        return Source.crend();
    }
}

template<>
struct CORE_API std::hash<UString>
{
    size_t operator()(const UString& str) const
    {
        return 1;
    }
};

template<>
struct CORE_API std::less<UString>
{
    bool operator()(const UString& lhs, const UString& rhs) const
    {
        return lhs.Compare(rhs);
    }
};

template <>
struct CORE_API fmt::formatter<UString> : fmt::formatter<int>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const UString& str, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "{0}", str.ToUtf8().Data());
    }
};