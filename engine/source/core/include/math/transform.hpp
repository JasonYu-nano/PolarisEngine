#pragma once

#include "math/quaternion.hpp"

#if ENABLE_TRANSFORM_INTRINSICS
#include "math/engine_simd.hpp"
#endif

namespace Engine
{
    struct alignas(16) CORE_API Transform
    {
        Transform()
            : Rotation(0, 0, 0, 1)
            , Location(0, 0, 0)
            , Scale(1, 1, 1)
        {}

        explicit Transform(const Quat& rotation)
            : Rotation(rotation)
            , Location(0, 0, 0)
            , Scale(1, 1, 1)
        {}

        explicit Transform(const Vector3f& location)
            : Rotation(0, 0, 0, 1)
            , Location(location)
            , Scale(1, 1, 1)
        {}

        Transform(const Quat& rotation, const Vector3f location, const Vector3f& scale)
            : Rotation(rotation)
            , Location(location)
            , Scale(scale)
        {}

        static void Multiply(const Transform& a, const Transform& b, Transform& result);

        Transform operator* (const Transform& other) const;

        Transform operator*= (const Transform& other);

        Quat Rotation;
        Vector3f Location;
        Vector3f Scale;

#if ENABLE_TRANSFORM_INTRINSICS
        VectorRegister Rotation;
        VectorRegister Location;
        VectorRegister Scale;
#endif
    };
}