#pragma once
#include "predefine/prerequisite.hpp"

#if PLATFORM_WINDOWS
#include "windows/windows_platform.hpp"
#endif

namespace Engine
{
    typedef CorePlatformType::int8      int8;
    typedef CorePlatformType::int16     int16;
    typedef CorePlatformType::int32     int32;
    typedef CorePlatformType::int64     int64;

    typedef CorePlatformType::uint8     uint8;
    typedef CorePlatformType::uint16    uint16;
    typedef CorePlatformType::uint32    uint32;
    typedef CorePlatformType::uint64    uint64;

    typedef CorePlatformType::size_t    size_t;
    typedef CorePlatformType::ptrdiff   ptrdiff;
    typedef CorePlatformType::intptr    intptr;
    typedef int32                       strsize;

    typedef CorePlatformType::ansi      ansi;
    typedef CorePlatformType::wchar     wchar;
    typedef CorePlatformType::char_t    char_t;
    typedef CorePlatformType::wcharsize wcharsize;

    typedef CorePlatformType::byte      byte;

#ifndef _T
#ifdef UNICODE
    #define _T(x) L ## x
#else
    #define _T(x) x
#endif
#endif

#ifdef OUT
    #undef OUT
#endif
    #define OUT

#ifdef IN
    #undef IN
#endif
    #define IN

#if defined(_DEBUG) || defined(DEBUG)
#ifndef ENGINE_DEBUG
    #define ENGINE_DEBUG
#endif
#endif

#ifdef META
#undef META
#endif

#ifdef META_REFLECT
    #define META(...) __attribute__((annotate(#__VA_ARGS__)))
#else
    #define META(...)
#endif

#ifndef NODISCARD
#define NODISCARD
#endif

}