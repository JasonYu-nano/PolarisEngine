#include <string>
#include "foundation/fixed_string.hpp"

namespace Engine
{
    FixedString FixedString::None()
    {
        return {};
    }

    FixedString::FixedString(const char_t* str)
    {
        MakeFixedString(FixedStringView{str, static_cast<int32>(CharUtils::StrLen(str))});
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

    void FixedString::MakeFixedString(FixedStringView view)
    {
        int32 numberCount = 0;
        Number = FixedStringHelper::SplitNumber(view.Data, view.Length);
        EntryId = StringEntryPool::Get().FindOrStore(view);
    }

    String FixedString::ToString() const
    {
        FixedStringView* entry = StringEntryPool::Get().Find(EntryId);
        if (entry == nullptr)
        {
            return String::Empty();
        }

        if (Number == SUFFIX_NUMBER_NONE)
        {
            return String(entry->Data);
        }
        else
        {
            return Format("{0}_{1}", entry->Data, SUFFIX_TO_ACTUAL(Number));
        }

        return String::Empty();
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