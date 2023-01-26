#pragma once

#include "definitions_core.hpp"
#include "foundation/details/string_entry_pool.hpp"
#include "foundation/string.hpp"

namespace Engine
{
    class CORE_API StringID
    {
    public:
        StringID() = default;

        StringID(const char* str);

        explicit StringID(const StringView& str);

        explicit StringID(const String& str);

        StringID(const StringID& other) = default;

        StringID& operator= (const StringID& other) = default;

        bool operator== (const StringID& other) const;

        bool operator!= (const StringID& other) const;

        uint32 GetHashCode() const
        {
            return GetCompressID() + GetNumber();
        }

        String ToString() const;

        uint32 GetCompressID() const {return EntryID.CompressID;}

        uint32 GetNumber() const;

    private:
        void MakeStringID(StringView view);

    private:
        StringEntryID EntryID;
        uint32 Number{ SUFFIX_NUMBER_NONE };
    };
}
