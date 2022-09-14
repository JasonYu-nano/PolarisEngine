#include "foundation/fixed_string.hpp"

namespace Engine
{
    FixedString::FixedString(const char* latain1)
    {
        MakeFixedString(static_cast<UStringView>(UString::FromLatin1(latain1)));
    }

    FixedString::FixedString(StringView str)
    {
        MakeFixedString(static_cast<UStringView>(UString::FromLatin1(str)));
    }

    FixedString::FixedString(UStringView str)
    {
        MakeFixedString(str);
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

    void FixedString::MakeFixedString(UStringView view)
    {
        Number = FixedStringHelper::SplitNumber(view);
        EntryId = StringEntryPool::Get().FindOrStore(view);
    }

    UString FixedString::ToString() const
    {
        UString* entry = StringEntryPool::Get().Find(EntryId);
        if (entry == nullptr)
        {
            return UString();
        }

        if (Number == SUFFIX_NUMBER_NONE)
        {
            return *entry;
        }

        return UString::Format("{0}_{1}", *entry, SUFFIX_TO_ACTUAL(Number));
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