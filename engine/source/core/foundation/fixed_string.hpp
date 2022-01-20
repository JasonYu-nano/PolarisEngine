#pragma once

#include "definitions_core.hpp"
#include "foundation/string.hpp"
#include "foundation/details/string_entry_pool.hpp"

namespace Engine
{
    class CORE_API FixedString
    {
    public:
        FixedString() = default;

        explicit FixedString(const char_t* str);

        FixedString(const FixedString& other);

        FixedString& operator= (const FixedString& other);

        bool operator== (const FixedString& other) const;

        bool operator!= (const FixedString& other) const;

        String ToString() const;

        FixedEntryId GetEntryId() const {return EntryId;}

        uint32 GetNumber() const;

    private:
        void MakeFixedString(FixedStringView view);

    private:
        FixedEntryId EntryId{ 0 };
        uint32 Number{ SUFFIX_NUMBER_NONE };
    };
}
