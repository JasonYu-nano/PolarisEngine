#include "foundation/fixed_string.hpp"

namespace Engine
{
    FixedString::FixedString(const char* str)
    {
        MakeFixedString(StringView(str));
    }

    FixedString::FixedString(const StringView& str)
    {
        MakeFixedString(str);
    }

    FixedString::FixedString(const String& str)
    {
        MakeFixedString(static_cast<StringView>(str));
    }

    FixedString::FixedString(const FixedString& other)
        : EntryId(other.EntryId)
        , Number(other.Number)
    {}

    FixedString &FixedString::operator=(const FixedString &other)
    {
        EntryId = other.EntryId;
        Number = other.Number;
        return *this;
    }

    bool FixedString::operator==(const FixedString &other) const
    {
        return EntryId == other.EntryId && Number == other.Number;
    }

    bool FixedString::operator!=(const FixedString &other) const
    {
        return EntryId != other.EntryId || Number != other.Number;
    }

    void FixedString::MakeFixedString(StringView view)
    {
        Number = FixedStringHelper::SplitNumber(view);
        EntryId = StringEntryPool::Get().FindOrStore(view);
    }

    String FixedString::ToString() const
    {
        String* entry = StringEntryPool::Get().Find(EntryId);
        if (entry == nullptr)
        {
            return String();
        }

        if (Number == SUFFIX_NUMBER_NONE)
        {
            return *entry;
        }

        return String::Format("{0}_{1}", *entry, SUFFIX_TO_ACTUAL(Number));
    }

    uint32 FixedString::GetNumber() const
    {
        if (Number == SUFFIX_NUMBER_NONE)
        {
            return 0u;
        }
        else
        {
            return SUFFIX_TO_ACTUAL(Number);
        }
    }
}