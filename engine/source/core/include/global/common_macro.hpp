#pragma once

#include "global/prerequisite.hpp"

#define INDEX_NONE (int32) -1

#define SMALL_FLOAT (1.e-8f)
#define KINDA_SMALL_FLOAT (1.e-4f)

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

#ifdef COMPILER_MSVC
    #define ALIGN(n) __declspec(align(n))
#else
    #define ALIGN(n) __attribute__((aligned(n)))
#endif