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

#if defined(ENABLE_TRANSFORM_INTRINSICS)
#undef ENABLE_TRANSFORM_INTRINSICS
#endif
#define ENABLE_TRANSFORM_INTRINSICS 0

#ifndef USE_BUILTIN_OFFSETOF
#define OFFSET_OF(struct, member) offsetof(struct, member)
#else
#define OFFSET_OF(struct, member) __builtin_offsetof(struct, member)
#endif