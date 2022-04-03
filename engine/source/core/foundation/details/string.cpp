//#include "precompiled_core.hpp"
#include "foundation/string.hpp"
#include "math/hash_helper.hpp"
#include "log/logger.hpp"

#ifdef UNICODE
#include <wchar.h>
#endif
#include "foundation/string_cast.hpp"

namespace Engine
{
    String::String(char_t val) : Internal(&val)
    {
    }

    String::String(const char_t* val) : Internal(val)
    {
    }

    String::String(InnerString&& val) : Internal(std::forward<InnerString>(val))
    {
    }

    String String::Concat(const String& lhs, const String& rhs)
    {
        return lhs.Internal + rhs.Internal;
    }

    String String::Concat(const String& lhs, const char_t* rhs)
    {
        return lhs.Internal + rhs;
    }

    String String::Empty()
    {
        static String empty = String(_T("\0"));
        return empty;
    }

    bool String::IsEmpty() const
    {
        return Internal.length() <= 0 || Internal[0] == '\0';
    }

    size_t String::Length() const
    {
        return Internal.length();
    }

    size_t String::Capacity() const
    {
        return Internal.capacity();
    }

    size_t String::GetHash() const
    {
        return HashHelper::FnvHash(Internal.c_str(), Internal.length());
    }

    bool String::IsValidIndex(size_t index) const
    {
        return Length() > index;
    }

    const char_t& String::At(size_t index) const
    {
        PL_ASSERT(index < Length());
        return Internal.at(index);
    }

    const char_t* String::Data() const
    {
        return &At(0);
    }

    bool String::EndWith(const char_t* value) const
    {
        size_t length = Length();
        size_t valueLength = CharUtils::StrLen(value);
        if (valueLength <= 0 || length < valueLength)
            return false;

        const char_t* start = (Data() + length - valueLength);
        while (valueLength > 0)
        {
            --valueLength;
            if (*(start + valueLength) != *(value + valueLength))
            {
                return false;
            }
        }
        return true;
    }

    bool String::StartWith(const char_t* value) const
    {
        size_t length = Length();
        size_t valueLength = CharUtils::StrLen(value);

        if (valueLength <= 0 || length < valueLength)
            return false;

        const char_t* start = Data();
        while (valueLength > 0)
        {
            --valueLength;
            if (*(start + valueLength) != *(value + valueLength))
            {
                return false;
            }
        }
        return true;
    }

    String& String::Append(const char_t* str)
    {
        Internal.append(str);
        return *this;
    }

    String& String::Append(const String& str)
    {
        return Append(str.Data());
    }

    inline String String::Substr(size_t start, size_t size) const
    {
        return Internal.substr(start, size);
    }

    inline size_t String::FindFirst(const String& delims, size_t pos) const
    {
        return Internal.find_first_of(delims.Internal, pos);
    }

    size_t String::FindLast(const String& delims, size_t pos) const
    {
        return Internal.find_last_of(delims.Internal, pos);
    }

    DynamicArray<String> String::Split(const char_t* delims) const
    {
        DynamicArray<String> ret;
        size_t first = 0;

        while (first < Length())
        {
            const auto second = FindFirst(delims, first);

            if (first != second)
            {
                ret.Add(Substr(first, second - first));
            }

            if (second == InvalidIndex)
            {
                break;
            }

            first = second + 1;
        }

        return ret;
    }

    String::operator TStringView() const
    {
        return TStringView(this->Data(), this->Length());
    }

    const char_t* String::operator*() const
    {
        return Length() > 0 ? Data() : _T("");
    }

    String operator+(const String& lhs, const String& rhs)
    {
        return String::Concat(lhs, rhs);
    }

    bool operator==(const String& lhs, const String& rhs)
    {
        return lhs.Internal == rhs.Internal;
    }

    bool operator!=(const String& lhs, const String& rhs)
    {
        return lhs.Internal != rhs.Internal;
    }

    bool String::operator==(const char_t *other)
    {
        return Internal == other;
    }

    bool String::operator!=(const char_t *other)
    {
        return Internal != other;
    }

    String::operator const ansi*() const
    {
#ifdef UNICODE
        return W2A(Data());
#else
        return Data();
#endif
    }

    String::operator const wchar*() const
    {
#ifdef UNICODE
        return Data();
#else
        return A2W(Data());
#endif
    }
}