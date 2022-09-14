#pragma once

#include "global.hpp"

using namespace Engine;

#define MIN_UINT8       ((uint8)        0x00)
#define MIN_UINT16      ((uint16)       0x0000)
#define MIN_UINT32      ((uint32)       0x00000000)
#define MIN_UINT64       ((uint64)       0x0000000000000000)
#define MIN_INT8        ((int8)         -128)
#define MIN_INT16       ((int16)        -32768)
#define MIN_INT32       ((int32)        0x80000000)
#define MIN_INT64       ((int64)        0x8000000000000000)

#define MAX_UINT8       ((uint8)        0xff)
#define MAX_UINT16      ((uint16)       0xffff)
#define MAX_UINT32      ((uint32)       0xffffffff)
#define MAX_UINT64      ((uint64)       0xffffffffffffffff)
#define MAX_INT8        ((int8)         0x7f)
#define MAX_INT16       ((int16)        0x7fff)
#define MAX_INT32       ((int32)        0x7fffffff)
#define MAX_INT64       ((int64)        0x7fffffffffffffff)

#define MIN_FLT         (1.175494351e-38F)
#define MAX_FLT         (3.402823466e+38F)
#define MIN_DBL         (2.2250738585072014e-308)
#define MAX_DBL         (1.7976931348623158e+308)

#if PLATFORM_WINDOWS
    #define MAX_DWORD   MAX_UINT32
#endif

namespace Engine
{
    template <typename T>
    struct NumericLimits
    {
    };

    template <>
    struct NumericLimits<uint8>
    {
        static constexpr uint8 Min()
        {
            return MIN_UINT8;
        }

        static constexpr uint8 Max()
        {
            return MAX_UINT8;
        }
    };

    template <>
    struct NumericLimits<uint16>
    {
        static constexpr uint16 Min()
        {
            return MIN_UINT16;
        }

        static constexpr uint16 Max()
        {
            return MAX_UINT16;
        }
    };

    template <>
    struct NumericLimits<uint32>
    {
        static constexpr uint32 Min()
        {
            return MIN_UINT32;
        }

        static constexpr uint32 Max()
        {
            return MAX_UINT32;
        }
    };

    template <>
    struct NumericLimits<uint64>
    {
        static constexpr uint64 Min()
        {
            return MIN_UINT64;
        }

        static constexpr uint64 Max()
        {
            return MAX_UINT64;
        }
    };

    template <>
    struct NumericLimits<int8>
    {
        static constexpr int8 Min()
        {
            return MIN_INT8;
        }

        static constexpr int8 Max()
        {
            return MAX_INT8;
        }
    };

    template <>
    struct NumericLimits<int16>
    {
        static constexpr int16 Min()
        {
            return MIN_INT16;
        }

        static constexpr int16 Max()
        {
            return MAX_INT16;
        }
    };

    template <>
    struct NumericLimits<int32>
    {
        static constexpr int32 Min()
        {
            return MIN_INT32;
        }

        static constexpr int32 Max()
        {
            return MAX_INT32;
        }
    };

    template <>
    struct NumericLimits<int64>
    {
        static constexpr int64 Min()
        {
            return MIN_INT64;
        }

        static constexpr int64 Max()
        {
            return MAX_INT64;
        }
    };
}