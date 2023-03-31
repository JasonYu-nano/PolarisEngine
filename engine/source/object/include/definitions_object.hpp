#pragma once

#include "global.hpp"

#ifdef PL_SHARED
#ifdef OBJECT_EXPORT
#define OBJECT_API DLLEXPORT
#else
#define OBJECT_API DLLIMPORT
#endif
#else
#define OBJECT_API
#endif