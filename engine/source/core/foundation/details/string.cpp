//#include "precompiled_core.hpp"
#include "foundation/string.hpp"
#include "math/hash_helper.hpp"
#include "log/logger.hpp"

#ifdef UNICODE
#include <wchar.h>
#endif

namespace Engine
{
    String::String(tchar val) : Internal(&val)
    {
    }

    String::String(const tchar* val) : Internal(val)
    {
    }

    String::String(InnerString&& val) : Internal(std::forward<InnerString>(val))
    {
    }

    String String::Concat(const String& lhs, const String& rhs)
    {
        return lhs.Internal + rhs.Internal;
    }

    String String::Concat(const String& lhs, const tchar* rhs)
    {
        return lhs.Internal + rhs;
    }

    String String::Empty()
    {
        static String empty = String(TC("\0"));
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

    const tchar& String::At(size_t index) const
    {
        //PL_ASSERT(index < Length());
        PL_FATAL("Engine", TC("index is invalid"));
        return Internal.at(index);
    }

    const tchar* String::Data() const
    {
        return &At(0);
    }

    bool String::EndWith(const tchar* value) const
    {
        size_t length = Length();
        size_t valueLength = CharUtils::StrLen(value);
        if (valueLength <= 0 || length < valueLength)
            return false;

        const tchar* start = (Data() + length - valueLength);
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

    bool String::StartWith(const tchar* value) const
    {
        size_t length = Length();
        size_t valueLength = CharUtils::StrLen(value);

        if (valueLength <= 0 || length < valueLength)
            return false;

        const tchar* start = Data();
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

    String& String::Append(const tchar* str)
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

    Vector<String> String::Split(const tchar* delims) const
    {
        std::vector<String> ret;
        size_t first = 0;

        while (first < Length())
        {
            const auto second = FindFirst(delims, first);

            if (first != second)
                ret.emplace_back(Substr(first, second - first));

            if (second == InvalidIndex)
                break;

            first = second + 1;
        }

        return ret;
    }

    String::operator TStringView() const
    {
        return TStringView(this->Data(), this->Length());
    }

    const tchar* String::operator*() const
    {
        return Length() > 0 ? Data() : TC("");
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

    bool String::operator==(const tchar *other)
    {
        return Internal == other;
    }

    bool String::operator!=(const tchar *other)
    {
        return Internal != other;
    }
}