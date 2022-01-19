#pragma once
#include <string>
#include <ostream>
#include "fmt/core.h"
#include "spdlog/fmt/fmt.h"
#include "definitions_core.hpp"
#include "foundation/array.hpp"
#include "foundation/string_view.hpp"
#include "foundation/char_utils.hpp"

namespace Engine
{
#ifdef Format
    #undefine Format
#endif
    #define Format(context, ...) fmt::format(TC(context), __VA_ARGS__)

    class CORE_API String
    {
        typedef std::basic_string<tchar, std::char_traits<tchar>, std::allocator<tchar>> InnerString;
    public:
        String() = default;
        String(const String& str) = default;
        String(String&& str) = default;
        String(tchar val);
        String(const tchar* val);
        String(InnerString&& val);

        static String Concat(const String& lhs, const String& rhs);
        static String Concat(const String& lhs, const tchar* rhs);

    public:

        static String Empty();

        bool IsEmpty() const;

        inline size_t Length() const;

        inline size_t Capacity() const;

        size_t GetHash() const;

        inline bool IsValidIndex(size_t index) const;

        const tchar& At(size_t index) const;

        const tchar* Data() const;

        String ToUpper() { return *this; }

        String ToLower() { return *this; }

        String Trim() { return *this; }

        bool EndWith (const tchar* value) const;

        bool StartWith (const tchar* value) const;

        String& Append(const tchar* str);

        String& Append(const String& str);

        inline String Substr(size_t start, size_t size) const;

        inline size_t FindFirst(const String& delims, size_t pos = std::string::npos) const;

        size_t FindLast(const String& delims, size_t pos = std::string::npos) const;

        Vector<String> Split(const tchar* delims) const;

    public:
        operator TStringView() const;

#ifdef UNICODE
        operator std::wstring() const { return Internal; }
#else
        operator std::string() const { return Internal; }
#endif

        const tchar* operator*() const;

        String& operator=(const String& lhs) = default;

        String& operator=(String&& lhs) = default;

        String& operator+=(const tchar* rhs);

        friend CORE_API String operator+(const String& lhs, const String& rhs);

        friend CORE_API bool operator==(const String& lhs, const String& rhs);

        friend CORE_API bool operator!=(const String& lhs, const String& rhs);

        bool operator== (const tchar* other);

        bool operator!= (const tchar* other);

#ifdef UNICODE
        friend CORE_API std::wostream& operator<<(std::wostream& out, const String& str) { out << *str; return out; }
#else
        friend ENGINE_API std::ostream& operator<<(std::ostream& out, const String& str) { out << *str; return out; }
#endif

    public:
        static constexpr int32 InvalidIndex{ -1 };
        
    private:
        InnerString Internal;
    };
}

using namespace Engine;

template<>
struct CORE_API std::hash<String>
{
    size_t operator()(const String& str) const
    {
        return str.GetHash();
    }
};

template<>
struct CORE_API std::less<String>
{
    bool operator()(const String& lhs, const String& rhs) const
    {
        return CharUtils::StrCmp(*lhs, *rhs);
    }
};

template <>
struct CORE_API fmt::formatter<String> : fmt::formatter<int>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const String& str, FormatContext& ctx) 
    {
        return fmt::format_to(ctx.out(), "{0}", *str);
    }
};
