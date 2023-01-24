#include "foundation/details/string_entry_pool.hpp"

namespace Engine
{
    StringEntryID StringEntryPool::FindOrStore(const StringView& entry)
    {
        StringEntryID id = AllocEntryID(entry);

        if (Find(id) == nullptr)
        {
            Store(id, entry);
        }
        return id;
    }

    void StringEntryPool::Store(StringEntryID id, const StringView& entry)
    {
        std::lock_guard lock(Mutex);
#if STRING_ID_CASE_SENSITIVE
        EntryPool.Add(id.DisplayID, String(entry.Data(), entry.Length()));
#else
        EntryPool.Add(id.CompressID, String(entry.Data(), entry.Length()));
#endif
    }

    String* StringEntryPool::Find(StringEntryID id)
    {
        std::shared_lock lock(Mutex);
#if STRING_ID_CASE_SENSITIVE
        return EntryPool.Find(id.DisplayID);
#else
        return EntryPool.Find(id.CompressID);
#endif
    }

    StringEntryID StringEntryPool::AllocEntryID(const StringView& entry)
    {
        StringEntryID id;
#if STRING_ID_CASE_SENSITIVE
        id.DisplayID = CalcDisplayID(entry);
#endif
        id.CompressID = CalcCompressID(entry);
        return id;
    }

    uint32 StringEntryPool::CalcCompressID(const StringView& view)
    {
        char lowerStr[MAX_ENTRY_LENGTH];
        int32 len = view.Length();
        const char* data = view.Data();
        for (int32 idx = 0; idx < len; ++idx)
        {
            lowerStr[idx] = CharTraits<char>::ToLowerLatin1(data[idx]);
        }

        return CityHash::CityHash32(reinterpret_cast<const char*>(lowerStr), len * sizeof(char));
    }

    uint32 StringEntryPool::CalcDisplayID(const StringView& view)
    {
        return CityHash::CityHash32(view.Data(), view.Length() * sizeof(char));
    }
}