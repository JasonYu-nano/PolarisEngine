#pragma once

#include "global/prerequisite.hpp"

#define INDEX_NONE (int32) -1

#ifdef COMPILER_MSVC
    #define LIKELY(expr)    expr
    #define UNLIKELY(expr)  expr
#elif defined(COMPILER_GCC)
    #define LIKELY(expr)    __builtin_expect(!!(expr), true)
    #define UNLIKELY(expr)  __builtin_expect(!!(expr), false)
#else
    #define LIKELY(expr)    expr
    #define UNLIKELY(expr)  expr
#endif