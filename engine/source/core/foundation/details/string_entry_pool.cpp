#include "string_entry_pool.hpp"

namespace Engine
{
    void StringEntryPool::Store(uint64 entryId, String entry)
    {
        std::lock_guard lock(Mutex);
    }

    void StringEntryPool::Find(uint64 entryId)
    {
        std::shared_lock lock(Mutex);
        String* entry = EntryPool.Find(entryId);
    }
}