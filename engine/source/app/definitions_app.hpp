#pragma once
#include "predefine/platform.hpp"

#ifdef SHARED_LIB
#ifdef APP_EXPORT
    #define APP_API DLLEXPORT
#else
    #define APP_API DLLIMPORT
#endif
#else
    #define APP_API
#endif