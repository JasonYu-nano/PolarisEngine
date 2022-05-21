#pragma once
#include "predefine/details/platform_type.hpp"

namespace Engine
{
    struct WinPlatformType : public PlatformType
    {
        #ifdef _WIN64
            typedef uint64          size_t;
            typedef int64           ptrdiff;
            typedef int64           intptr;
        #else
            typedef uint32          size_t;
            typedef int32           ptrdiff;
            typedef int32           intptr;
        #endif
            typedef uint16          wcharsize;
    };

    typedef WinPlatformType CorePlatformType;

    #define DLLIMPORT __declspec(dllimport)
    #define DLLEXPORT __declspec(dllexport)

    #define NODISCARD [[nodiscard]]
}