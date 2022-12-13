#pragma once

#include "definitions_core.hpp"
#include "foundation/details/string_entry_pool.hpp"
#include "foundation/string.hpp"

namespace Engine
{
    class CORE_API FixedString
    {
    public:
        FixedString() = default;

        FixedString(const char* str);

        FixedString(StringView str);

        FixedString(const String& str);

        FixedString(const FixedString& other);

        FixedString& operator= (const FixedString& other);

        bool operator== (const FixedString& other) const;

        bool operator!= (const FixedString& other) const;

        String ToString() const;

        FixedEntryId GetEntryId() const {return EntryId;}

        uint32 GetNumber() const;

    private:
        void MakeFixedString(StringView view);

    private:
        FixedEntryId EntryId{ 0 };
        uint32 Number{ SUFFIX_NUMBER_NONE };
    };

    template <>
    inline uint32 GetHashCode(const FixedString& name)
    {
        return (uint32)(name.GetEntryId()) + ((uint32)(name.GetEntryId() >> 32) * 23) + name.GetNumber();
    }
}
