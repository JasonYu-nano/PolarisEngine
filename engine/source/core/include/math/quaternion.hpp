#pragma once

#include "definitions_core.hpp"

namespace Engine
{
    struct ALIGN(16) CORE_API Quat
    {
        Quat(float x, float y, float z, float w);

        float X = 0.0f;
        float Y = 0.0f;
        float Z = 0.0f;
        float W = 0.0f;
    };
}