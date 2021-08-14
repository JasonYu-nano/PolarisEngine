#pragma once
#include "predefine/prerequisite.hpp"

#ifdef SHARED_LIB
#if PLATFORM_WINDOWS
    #define CORE_API __declspec(dllexport)
#endif
#else
    #define CORE_API
#endif
