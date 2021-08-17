#pragma once
#include "predefine/platform.hpp"

#ifdef SHARED_LIB
#ifdef CORE_EXPORT
    #define CORE_API DLLEXPORT
#else
    #define CORE_API DLLIMPORT
#endif
#else
    #define CORE_API
#endif