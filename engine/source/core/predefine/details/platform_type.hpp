#pragma once

namespace Engine
{
    struct PlatformType
    {
        typedef unsigned char           uint8;
        typedef unsigned short int      uint16;
        typedef unsigned int            uint32;
        typedef unsigned long long      uint64;

        typedef signed char             int8;
        typedef signed short int        int16;
        typedef signed int              int32;
        typedef signed long long        int64;

        typedef char                    ansi;
        typedef wchar_t                 wchar;

        #ifdef UNICODE
        typedef wchar_t                 char_t;
        #else
        typedef char                    char_t;
        #endif

        typedef unsigned char           byte;
    };
}