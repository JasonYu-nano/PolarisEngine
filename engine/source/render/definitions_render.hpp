#pragma once

#include "predefine/platform.hpp"

#ifdef PL_SHARED
#ifdef RENDER_EXPORT
    #define RENDER_API DLLEXPORT
#else
    #define RENDER_API DLLIMPORT
#endif
#else
    #define RENDER_API
#endif