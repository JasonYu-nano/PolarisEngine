#include "foundation/ustring.hpp"
#include "foundation/encoding.hpp"
#include "foundation/char_utils.hpp"
#include "foundation/details/string_algorithm.hpp"

#define STR_INLINE_BUFFER_SIZE 500

#define COPY_CHAR_TO_UCHAR_ARRAY(dest, src, len) \
    UChar* _destPtr = dest.Data();\
    const char* _current = src;\
    strsize _len = len;\
    while (_len--)\
    {*_destPtr++ = (UChar)*_current++;}

namespace Engine
{
    UString::UString(UChar c)
    {
        Source.Reserve(2);
        Source.Add(c);
        Source.Add(u'\0');
    }

    UString::UString(const char* utf8, strsize len)
        : UString(FromUtf8(StringView(utf8, len < 0 ? CharUtils::Length(utf8) : len)))
    {}

    UString::UString(const UChar* unicode, strsize len)
    {
        if (!unicode)
        {
            Source.Clear();
        }
        else
        {
            len = len >= 0 ? len : CharUtils::Length(unicode);
            if (!len)
            {

            }
            else
            {
                Source.Reserve(len + 1);
                Source.Add(unicode, len);
                Source.Add(u'\0');
            }
        }
    }

    UString::UString(strsize count, UChar c)
    {
        if (count < 0)
        {
            return;
        }

        Source.Resize(count, c);
    }

    UString UString::FromLatin1(StringView str)
    {
        strsize len = str.Length();
        UString ret;
        ret.Resize(len);
        PL_ASSERT(ret.Capacity() >= len);

        COPY_CHAR_TO_UCHAR_ARRAY(ret.Source, str.Data(), len);

        return ret;
    }

    UString UString::FromUtf8(StringView view)
    {
        if (view.IsEmpty())
        {
            return UString();
        }

        strsize len = view.Length();

        UString ret;
        int32 destLength = 0;
        if (Utf8::ToUnicode(nullptr, 0, &destLength, view.Data(), len))
        {
            ret.Resize(destLength);

            if (Utf8::ToUnicode(UCHAR_TO_UTF16(ret.Data()), ret.Capacity() + 1, &destLength, view.Data(), len))
            {
                return ret;
            }
        }

        return ret;
    }

    UString UString::FromUtf16(const char16_t* unicode, strsize len)
    {
        len = len >= 0 ? len : CharUtils::Length(unicode);
        //TODO:
        return UString(reinterpret_cast<const UChar*>(unicode), len);
    }

    UString UString::FromUtf32(const char32_t* ucs4, strsize len)
    {
        if (!ucs4 || len < 0)
        {
            return UString();
        }

        DynamicArray<char16_t, InlineAllocator<STR_INLINE_BUFFER_SIZE>> buffer;

        int32 destLength = 0;
        if (Utf32::ToUnicode(nullptr, 0, &destLength, ucs4, len))
        {
            buffer.Resize(destLength + 1, u'\0');

            if (Utf32::ToUnicode(buffer.Data(), buffer.Capacity(), &destLength, ucs4, len))
            {
                return UString(K_UTF16_TO_UCHAR(buffer.Data()), len);;
            }
        }

        return UString();
    }

    UString UString::FromStdString(const std::string& str)
    {
        //TODO: Check length size
        return FromUtf8(StringView(str.data(), static_cast<strsize>(str.length())));
    }

    DynamicArray<char> UString::ToLatin1() const
    {
        strsize size = Length() + 1;
        DynamicArray<char> ret(size);
        char* dest = ret.Data();
        const UChar* src = Data();
        while (size--)
        {
            *dest++ = (char)*src++;
        }

        return ret;
    }

    DynamicArray<char> UString::ToUtf8() const
    {
        strsize len = Length();

        int32 destLength = 0;
        if (Utf16::ToUtf8(nullptr, 0, &destLength, K_UCHAR_TO_UTF16(Data()), len))
        {
            DynamicArray<char> utf8('\0', destLength + 1);

            if (Utf16::ToUtf8(utf8.Data(), utf8.Capacity(), &destLength, K_UCHAR_TO_UTF16(Data()), len))
            {
                return utf8;
            }
        }

        return DynamicArray<char>();
    }

    std::string UString::ToStdString() const
    {
        DynamicArray<char> utf8 = ToUtf8();
        if (utf8.Size() >= 1)
        {
            return std::string(utf8.Data(), utf8.Size() - 1);
        }

        return std::string();
    }

    strsize UString::Length() const
    {
        return IsNull() ? 0 : Source.Size() - 1;
    }

    strsize UString::Capacity() const
    {
        strsize capacity = Source.Capacity();
        return capacity > 0 ? capacity - 1 : capacity;
    }

    bool UString::IsNull() const
    {
        return Source.Size() == 0;
    }

    bool UString::IsEmpty() const
    {
        return Source.Size() <= 1;
    }

    void UString::Resize(strsize len)
    {
        if (len < 0)
        {
            len = 0;
        }

        Source.Resize(len + 1);
        Source[len] = u'\0';
    }

    void UString::Truncate(strsize pos)
    {
        if (pos < Length())
        {
            Resize(pos);
        }
    }

    bool UString::StartsWith(UStringView latin1, ECaseSensitivity cs) const
    {
        strsize compareLen = latin1.Length();
        if (IsNull() || Length() < compareLen)
        {
            return false;
        }

        if (cs == ECaseSensitivity::Sensitive)
        {
            return CharUtils::Compare(Data(), latin1.Data(), compareLen) == 0;
        }
        else
        {
            return CharUtils::UCompareInsensitive(Data(), latin1.Data(), compareLen) == 0;
        }
    }

    bool UString::StartsWith(const char* latin1, ECaseSensitivity cs) const
    {
        strsize compareLen = CharUtils::Length(latin1);
        if (IsNull() || Length() < compareLen)
        {
            return false;
        }

        if (cs == ECaseSensitivity::Sensitive)
        {
            return CharUtils::Compare(Data(), latin1, compareLen) == 0;
        }
        else
        {
            return CharUtils::UCompareInsensitive(Data(), latin1, compareLen) == 0;
        }
    }

    bool UString::EndsWith(UStringView latin1, ECaseSensitivity cs) const
    {
        strsize compareLen = latin1.Length();
        if (IsNull() || Length() < compareLen)
        {
            return false;
        }

        if (cs == ECaseSensitivity::Sensitive)
        {
            return CharUtils::Compare(Data() + Length() - compareLen, latin1.Data(), compareLen) == 0;
        }
        else
        {
            return CharUtils::UCompareInsensitive(Data() + Length() - compareLen, latin1.Data(), compareLen) == 0;
        }
    }

    bool UString::EndsWith(const char* latin1, ECaseSensitivity cs) const
    {
        strsize compareLen = CharUtils::Length(latin1);
        if (IsNull() || Length() < compareLen)
        {
            return false;
        }

        if (cs == ECaseSensitivity::Sensitive)
        {
            return CharUtils::Compare(Data() + Length() - compareLen, latin1, compareLen) == 0;
        }
        else
        {
            return CharUtils::UCompareInsensitive(Data() + Length() - compareLen, latin1, compareLen) == 0;
        }
    }

    UString& UString::Append(const UChar* str, strsize len)
    {
        len = len >= 0 ? len : CharUtils::Length(str);

        if (str && len > 0)
        {
            if (IsNull())
            {
                Source.Reserve(len + 1);
                Source.Add(str, len);
                Source.Add(u'\0');
            }
            else
            {
                Source.Insert(Length(), str, len);
            }
        }
        return *this;
    }

    UString& UString::Insert(strsize pos, const UChar* str, strsize len)
    {
        if (pos >= 0 && pos < Length() && len > 0)
        {
            Source.Insert(pos, str, len);
        }

        return *this;
    }

    UString& UString::Remove(strsize pos, strsize num)
    {
        strsize len = Length();
        if (pos < 0)
        {
            pos += len;
        }
        if (pos < 0 || pos >= len)
        {
        }
        else if (num >= len - pos)
        {
            Resize(pos);
        }
        else if (num > 0)
        {
            Source.Remove(pos, pos + num - 1);
        }

        return *this;
    }

    UString& UString::Remove(const char* latin1, ECaseSensitivity cs)
    {
        strsize sl = CharUtils::Length(latin1);

        DynamicArray<UChar> unicode(sl + 1);
        PL_ASSERT(unicode.Capacity() >= sl);
        COPY_CHAR_TO_UCHAR_ARRAY(unicode, latin1, sl);

        UStringView view(unicode.Data(), sl);
        strsize from = 0;
        do
        {
            strsize pos = Private::FindString((UStringView)*this, from, view, cs);
            if (pos >= 0)
            {
                Source.Remove(pos, pos + sl - 1);
                from = pos;
            }
            else
            {
                break;
            }
        }
        while (true);

        return *this;
    }

    UString& UString::Remove(const UString& str, ECaseSensitivity cs)
    {
        strsize from = 0;
        do
        {
            strsize pos = Private::FindString((UStringView)*this, from, (UStringView)str, cs);
            if (pos >= 0)
            {
                Source.Remove(pos, pos + str.Length() - 1);
                from = pos;
            }
            else
            {
                break;
            }
        }
        while (true);

        return *this;
    }

    UString& UString::Remove(UChar ch, ECaseSensitivity cs)
    {
        strsize from = 0;
        do
        {
            strsize pos = Private::FindChar((UStringView)*this, from, ch, cs);
            if (pos >= 0)
            {
                Source.Remove(pos, pos);
                from = pos;
            }
            else
            {
                break;
            }
        }
        while (true);

        return *this;
    }

    bool UString::IsUpper() const
    {
        for (const auto& ch : *this)
        {
            if (!ch.IsUpper())
            {
                return false;
            }
        }
        return true;
    }

    bool UString::IsLower() const
    {
        for (const auto& ch : *this)
        {
            if (!ch.IsLower())
            {
                return false;
            }
        }
        return true;
    }

    void UString::ToUpper()
    {
        for (auto& ch : *this)
        {
            ch.ToUpper();
        }
    }

    void UString::ToLower()
    {
        for (auto& ch : *this)
        {
            ch.ToLower();
        }
    }

    void UString::Chop(strsize n)
    {
        if (n >= Length())
        {
            Clear();
        }
        else if (n > 0)
        {
            Source.Remove(Length() - n, Length() - 1);
        }
    }

    UString UString::Chopped(strsize n)
    {
        if (n < 0 || n >= Length())
        {
            return UString();
        }

        return UString(Data(), Length() - n);
    }

    strsize UString::IndexOf(UStringView str, ECaseSensitivity cs) const
    {
        return Private::FindString((UStringView)*this, 0, str, cs);
    }

    strsize UString::IndexOf(const char* latin1, ECaseSensitivity cs) const
    {
        return Private::FindString((UStringView)*this, 0 , StringView(latin1, CharUtils::Length(latin1)), cs);
    }

    bool UString::Contains(UStringView str, ECaseSensitivity cs) const
    {
        strsize pos = 0;
        return Private::FindString((UStringView)*this, 0, str, cs) >= 0;
    }

    bool UString::Contains(const char* latin1, ECaseSensitivity cs) const
    {
        return false;
    }

    int32 UString::Count(const UString& str, ECaseSensitivity cs) const
    {
        strsize num = 0;
        strsize i = -1;
        UStringView view = static_cast<UStringView>(str);
        while ((i = Private::FindString((UStringView)*this, i + 1, view, cs)) != -1)
        {
            ++num;
        }
        return num;
    }

    int32 UString::Count(const char* latin1, ECaseSensitivity cs) const
    {
        strsize len = CharUtils::Length(latin1);

        DynamicArray<UChar> unicode(len + 1);
        PL_ASSERT(unicode.Capacity() >= len);
        COPY_CHAR_TO_UCHAR_ARRAY(unicode, latin1, len);

        strsize num = 0;
        strsize i = -1;
        UStringView view(unicode.Data(), len);
        while ((i = Private::FindString((UStringView)*this, i + 1, view, cs)) != -1)
        {
            ++num;
        }
        return num;
    }

    UString& UString::Fill(UChar ch, int32 num)
    {
        strsize len = Length();
        if (len == 0 && len == num)
        {
            return *this;
        }

        strsize size = num >= 0 ? num + 1 : len + 1;
        Source.Resize(size);

        while (size > 0)
        {
            --size;
            Source[size] = ch;
        }
        return *this;
    }

    UString UString::Repeated(int32 times) const
    {
        if (IsEmpty() || times == 1)
        {
            return *this;
        }

        if (times <= 0)
        {
            return UString();
        }

        strsize len = Length();
        strsize resultSize = len * times;
        UString ret;
        ret.Source.Reserve(resultSize + 1);
        if (ret.Capacity() == resultSize)
        {
            const UChar* raw = Data();
            while (times > 0)
            {
                ret.Source.Add(raw, len);
                --times;
            }
        }
        ret.Source.Add('\0');

        return ret;
    }

    UString UString::Trimmed() const
    {
        const UChar* start = Data();
        const UChar* end = Data() + Length() - 1;

        while (start < end && start->IsSpace())
        {
            ++start;
        }

        while (start < end && end[-1].IsSpace())
        {
            --end;
        }
        return UString(start, static_cast<strsize>(end - start));
    }

    int32 UString::Compare(UStringView other, ECaseSensitivity cs) const
    {
        if (cs == ECaseSensitivity::Sensitive)
        {
            return CharUtils::Compare(Data(), Length(), other.Data(), other.Length());
        }
        else
        {
            return CharUtils::UCompareInsensitive(Data(), Length(), other.Data(), other.Length()) == 0;
        }
    }

    int32 UString::Compare(const char* other, ECaseSensitivity cs) const
    {
        if (cs == ECaseSensitivity::Sensitive)
        {
            return CharUtils::Compare(Data(), Length(), other, CharUtils::Length(other));
        }
        else
        {
            return CharUtils::UCompareInsensitive(Data(), Length(), other, CharUtils::Length(other));
        }
    }

    int32 UString::Compare(const UString& other, ECaseSensitivity cs) const
    {
        return Compare((UStringView)other, cs);
    }

    int32 UString::Compare(UChar other, ECaseSensitivity cs) const
    {
        return Compare(UStringView(&other, 1), cs);
    }
}