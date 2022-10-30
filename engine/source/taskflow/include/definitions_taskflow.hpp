#pragma once

#include "global.hpp"

#ifdef PL_SHARED
#ifdef TASKFLOW_EXPORT
#define TASKFLOW_API DLLEXPORT
#else
#define TASKFLOW_API DLLIMPORT
#endif
#else
#define TASKFLOW_API
#endif