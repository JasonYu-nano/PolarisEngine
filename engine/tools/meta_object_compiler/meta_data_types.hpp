#pragma once

#include "foundation/string.hpp"

namespace Engine
{
    struct MetaFlagName
    {
        constexpr static const char* MF_ReadOnly = "ReadOnly";
        constexpr static const char* MF_ReadWrite = "ReadWrite";
    };
}