#pragma once

#include "predefine/platform.hpp"
#include "foundation/type_traits.hpp"

namespace Engine
{
    inline uint32 GetHashCode(const int8 value)
    {
        return value;
    }

    inline uint32 GetHashCode(const uint8 value)
    {
        return value;
    }

    inline uint32 GetHashCode(const int16 value)
    {
        return value;
    }

    inline uint32 GetHashCode(const uint16 value)
    {
        return value;
    }

    inline uint32 GetHashCode(const int32 value)
    {
        return value;
    }

    inline uint32 GetHashCode(const uint32 value)
    {
        return value;
    }

    inline uint32 GetHashCode(const int64 value)
    {
        return (uint32)value + ((uint32)(value >> 32) * 23);
    }

    inline uint32 GetHashCode(const uint64 value)
    {
        return (uint32)value + ((uint32)(value >> 32) * 23);
    }

    inline uint32 GetHashCode(const float value)
    {
        return *(uint32*)&value;
    }

    inline uint32 GetHashCode(const double value)
    {
        return GetHashCode(*(uint64*)&value);
    }

    inline uint32 GetHashCode(const void* value)
    {
        std::uintptr_t ptrInt = reinterpret_cast<std::uintptr_t>(value);

        return GetHashCode(ptrInt);
    }
}
