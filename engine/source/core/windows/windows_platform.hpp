#pragma once
#include "core/predefine/details/platform_type.hpp"

namespace Engine
{
    struct WinPlatformType : public PlateformType
    {
        #ifdef _WIN64
            typedef uint64          size_t;
        #else
            typedef unsigned int    size_t;
        #endif
    };

    typedef WinPlatformType CorePlatformType;

    #define DLLIMPORT __declspec(dllimport)
    #define DLLEXPORT __declspec(dllexport)
}