#include "foundation/details/string_entry_pool.hpp"

namespace Engine
{
    FixedEntryId StringEntryPool::FindOrStore(const UStringView& entry)
    {
        FixedEntryId entryId = AllocEntryId(entry);

        if (Find(entryId) == nullptr)
        {
            std::lock_guard lock(Mutex);
            EntryPool.Add(entryId, UString(entry.Data(), entry.Length()));
        }
        return entryId;
    }

    FixedEntryId StringEntryPool::Store(const UStringView& entry)
    {
        FixedEntryId entryId = AllocEntryId(entry);
        std::lock_guard lock(Mutex);
        EntryPool.Add(entryId, UString(entry.Data(), entry.Length()));
        return entryId;
    }

    UString* StringEntryPool::Find(FixedEntryId entryId)
    {
        std::shared_lock lock(Mutex);
        return EntryPool.Find(entryId);
    }

    FixedEntryId StringEntryPool::AllocEntryId(const UStringView& entry)
    {
        FixedEntryId entryId = 0;
        entryId = GetLowerCaseHash(entry);
        return entryId;
    }
}