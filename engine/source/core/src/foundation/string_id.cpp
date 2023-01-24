#include "foundation/string_id.hpp"

namespace Engine
{
    StringID::StringID(const char* str)
    {
        MakeStringID(StringView(str));
    }

    StringID::StringID(const StringView& str)
    {
        MakeStringID(str);
    }

    StringID::StringID(const String& str)
    {
        MakeStringID(static_cast<StringView>(str));
    }

    bool StringID::operator==(const StringID &other) const
    {
        return EntryID.CompressID == other.EntryID.CompressID && Number == other.Number;
    }

    bool StringID::operator!=(const StringID &other) const
    {
        return EntryID.CompressID != other.EntryID.CompressID || Number != other.Number;
    }

    void StringID::MakeStringID(StringView view)
    {
        Number = StringIDHelper::SplitNumber(view);
        EntryID = StringEntryPool::Get().FindOrStore(view);
    }

    String StringID::ToString() const
    {
        String* entry = StringEntryPool::Get().Find(EntryID);
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

    uint32 StringID::GetNumber() const
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