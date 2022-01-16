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

        explicit FixedString(const wchar* str);

        explicit FixedString(const schar* str);

        FixedString(const FixedString& other);

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
