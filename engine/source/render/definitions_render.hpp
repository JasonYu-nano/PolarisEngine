#pragma once

#include "predefine/platform.hpp"

#ifdef SHARED_LIB
#ifdef RENDER_EXPORT
    #define RENDER_API DLLEXPORT
#else
    #define RENDER_API DLLIMPORT
#endif
#else
    #define RENDER_API
#endif