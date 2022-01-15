#pragma once

#include <shared_mutex>
#include "foundation/map.hpp"
#include "foundation/string.hpp"

namespace Engine
{
    constexpr int32 InitialBucketCount = 1 << 9;

    using StringEntryPoolAllocator = SetAllocator<HeapAllocator<uint32>, HeapAllocator<uint32>, 2, InitialBucketCount, 4>;

    class StringEntryPool
    {
        using TEntryPool = Map<uint64, String, MapDefaultHashFunc<uint64, String>, StringEntryPoolAllocator>;
    public:
        void Store(uint64 entryId, String entry);

        void Find(uint64 entryId);
    private:
        std::shared_mutex Mutex;
        TEntryPool EntryPool;
    };
}