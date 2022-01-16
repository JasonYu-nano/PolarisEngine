#include <string>
#include "foundation/fixed_string.hpp"

namespace Engine
{

    FixedString::FixedString(const wchar *str)
    {
        MakeFixedString(FixedStringView{true, static_cast<int32>(wcslen(str)), {.Wide = str} });
    }

    FixedString::FixedString(const schar *str)
    {
        MakeFixedString(FixedStringView{false, static_cast<int32>(strlen(str)), {.Ansi = str }});
    }

    FixedString::FixedString(const FixedString& other)
        : EntryId(other.EntryId)
        , Number(other.Number)
    {}

    void FixedString::MakeFixedString(FixedStringView view)
    {
        int32 numberCount = 0;
        if (view.IsWide)
        {
            Number = FixedStringHelper::SplitNumber(view.Wide, view.Length);
        }
        else
        {
            Number = FixedStringHelper::SplitNumber(view.Ansi, view.Length);
        }

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
            if (entry->IsWide)
            {
                return String(entry->Wide);
            }
            else
            {
                //TODO: support ansi to string
                return String::Empty();//entry->Ansi);
            }
        }
        else
        {
            if (entry->IsWide)
            {
                return Format("{0}_{1}", entry->Wide, SUFFIX_TO_ACTUAL(Number));
            }
            else
            {
                return String::Empty();//Format("{0}_{1}", entry->Ansi, SUFFIX_TO_ACTUAL(Number));
            }
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