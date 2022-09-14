#pragma once

#include "definitions_core.hpp"
#include "file_system/file_system_type.hpp"

namespace Engine
{
    enum class EFileAccess : uint32
    {
        None = 0,
        Read = 1 << 0,
        Write = 1 << 1,
        ReadWrite = 1 << 2
    };

    enum class EFileShareMode : uint32
    {
        None = 0,
        Read = 1 << 0,
        Write = 1 << 1,
        Delete = 1 << 2
    };

    class CORE_API IFileHandle
    {
    public:
        virtual ~IFileHandle() = default;

        virtual int64 GetSize() const = 0;

        virtual bool Read(uint8* dest, int64 size) = 0;
    };

    class CORE_API IFindFileHandle
    {
    public:
        virtual ~IFindFileHandle() = default;

        virtual bool FindNext(DirectoryEntry& entry) = 0;

    protected:
        DirectoryEntry Entry;
    };
}