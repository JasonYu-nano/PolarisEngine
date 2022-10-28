#pragma once

#if ENABLE_TRANSFORM_INTRINSICS

#include "math/transform.hpp"

namespace Engine
{

    Engine::Transform::Transform()
        : Rotation(VectorSetW(VectorZero(), 1.0f))
        , Translation(VectorZero())
        , Scale(VectorSetW(MakeVectorRegister(1.0f), 0.0f))
    {}
}

#endif