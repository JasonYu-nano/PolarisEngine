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

#define FILE_PATH 1

#define FILE_ID_INTERNAL(path, line) path##_##line
#define FILE_ID_COMBINE(path, line) FILE_ID_INTERNAL(path, line)
#define FILE_ID FILE_ID_COMBINE(FILE_PATH, __LINE__)

#define MOC_GENERATED_INTERNAL(fileId) MOC_GENERATED_##fileId
#define MOC_GENERATED_COMBINE(fileId) MOC_GENERATED_INTERNAL(fileId)

#if !defined(MOC_BUILDER)
#define MOC_GENERATED() MOC_GENERATED_COMBINE(FILE_ID)()
#else
#define MOC_GENERATED()
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
