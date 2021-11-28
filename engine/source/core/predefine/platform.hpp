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

    typedef CorePlatformType::ansi      schar;
    typedef CorePlatformType::wchar     wchar;
    typedef CorePlatformType::tchar     tchar;

    typedef CorePlatformType::byte      byte;

#ifdef NInterface
    #undef NInterface
#endif

    #define NInterface class

#if defined(TEXTS)
    #undef TEXTS
#endif

#ifdef UNICODE
    #define TC(x) L ## x
#else
    #define TC(x) x
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