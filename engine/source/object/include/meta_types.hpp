#pragma once

#include "global.hpp"
#include "foundation/string.hpp"

namespace Engine
{
#if defined(MOC_BUILDER)
    #define META(...) [[clang::annotate(#__VA_ARGS__)]]
#else
    #define META(...)
#endif

#define WITH_META_DATA 1

#if !defined(MOC_BUILDER)
    #define MOC_GENERATED(className) MOC_GENERATED_##className()
#else
    #define MOC_GENERATED(className)
#endif


    enum EMetaFlag
    {
        MF_None             = 0x00000000,
        MF_ReadOnly         = 0x00000001,
        MF_ReadWrite        = 0x00000002
    };

#if WITH_META_DATA
    enum class EMetaData : uint8
    {
        DisplayName,
        Tooltips,
    };
#endif
}
