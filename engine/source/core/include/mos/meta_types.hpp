#pragma once

#include <filesystem>
#include "global.hpp"
#include "foundation/string.hpp"

namespace Engine
{
#if defined(MOC_BUILDER)
    #define META(...) [[clang::annotate(#__VA_ARGS__)]]
#else
    #define META(...)
#endif

#define META_CLASS_GENERATED(className) META_CLASS_GENERATED_##className()

    enum EMetaFlag
    {
        MF_None             = 0x00000000,
        MF_ReadOnly         = 0x00000001,
        MF_ReadWrite        = 0x00000002
    };
}
