#pragma once

#include <filesystem>
#include "definitions_core.hpp"
#include "foundation/time.hpp"

namespace Engine
{
    enum class FileAttribute : uint32
    {
        Readonly      = 0x00000001,
        Hidden        = 0x00000002,
        System        = 0x00000004,
        Directory     = 0x00000010,
        Archive       = 0x00000020,
        Device        = 0x00000040,
        Normal        = 0x00000080,
        Temporary     = 0x00000100,
        SparseFile   = 0x00000200,
        ReparsePoint = 0x00000400,
        Invalid       = 0xFFFFFFFF,
    };

    struct CORE_API FileStat
    {
        Timestamp LastWriteTime = {};
        Timestamp LastAccessTime = {};
        Timestamp CreationTime = {};
        int64 FileSize = 0;
        bool ReadOnly = false;
        bool IsDirectory = false;
        bool IsValid = false;
    };

    class CORE_API DirectoryEntry
    {
    public:
        DirectoryEntry() = default;

        DirectoryEntry(FileStat stat, const UString& path)
            : Path(path), Stat(stat) {}

    private:
        UString Path;
        FileStat Stat;
    };
}