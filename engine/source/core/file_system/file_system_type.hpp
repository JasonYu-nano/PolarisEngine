#pragma once

#include <filesystem>
#include "definitions_core.hpp"

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
        std::timestamp LastWriteTime;
        int64 FileSize;
        FileAttribute Attributes;
        __std_fs_reparse_tag _Reparse_point_tag;
        int32 _Link_count;
        __std_fs_stats_flags _Available; // which fields are available
    };

    class CORE_API DirectoryEntry
    {
        FileStat Status;
    };
}