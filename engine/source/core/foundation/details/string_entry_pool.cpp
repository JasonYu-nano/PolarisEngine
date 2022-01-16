#include "string_entry_pool.hpp"

namespace Engine
{
    FixedEntryId StringEntryPool::FindOrStore(const FixedStringView& entry)
    {
        FixedEntryId entryId = AllocEntryId(entry);

        if (Find(entryId) == nullptr)
        {
            std::lock_guard lock(Mutex);
            EntryPool.Add(entryId, entry);
        }
        return entryId;
    }

    FixedEntryId StringEntryPool::Store(const FixedStringView &entry)
    {
        FixedEntryId entryId = AllocEntryId(entry);
        std::lock_guard lock(Mutex);
        EntryPool.Add(entryId, entry);
        return entryId;
    }

    FixedStringView* StringEntryPool::Find(FixedEntryId entryId)
    {
        std::shared_lock lock(Mutex);
        return EntryPool.Find(entryId);
    }

    FixedEntryId StringEntryPool::AllocEntryId(const FixedStringView &entry)
    {
        FixedEntryId entryId = 0;
        if (entry.IsWide)
        {
            entryId = GetLowerCaseHash(entry.Wide, entry.Length);
        }
        else
        {
            entryId = GetLowerCaseHash(entry.Ansi, entry.Length);
        }
        return entryId;
    }
}