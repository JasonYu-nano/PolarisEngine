#pragma once
#include "global.hpp"

#ifdef PL_SHARED
#ifdef CORE_EXPORT
    #define CORE_API DLLEXPORT
#else
    #define CORE_API DLLIMPORT
#endif
#else
    #define CORE_API
#endif