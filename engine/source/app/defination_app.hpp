#pragma once

#include "predefine/prerequisite.hpp"

#ifdef SHARED_LIB
#if PLATFORM_WINDOWS
    #define RENDER_API __declspec(dllimport)
    #define CORE_API __declspec(dllimport)
#endif
#else
    #define RENDER_API
    #define CORE_API
#endif