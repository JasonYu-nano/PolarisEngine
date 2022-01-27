#pragma once

#include "definitions_core.hpp"

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

        virtual uint64 GetSize() const = 0;

        virtual bool Read(uint8* dest, uint64 size) = 0;
    };
}