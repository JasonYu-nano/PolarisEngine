#if !ENABLE_TRANSFORM_INTRINSICS

#include "math/transform.hpp"

namespace Engine
{
    void Transform::Multiply(const Transform& a, const Transform& b, Transform& result)
    {
        Quat rotationB = b.Rotation;
        Vector3f scaleB = b.Scale;

        result.Rotation = a.Rotation * b.Rotation;
        result.Scale = a.Scale * b.Scale;
        result.Translation = rotationB * (scaleB * a.Translation) + b.Translation;
    }

    Transform Transform::operator*(const Transform& other) const
    {
        Transform result;
        Multiply(*this, other, result);
        return result;
    }

    Transform Transform::operator*=(const Transform& other)
    {
        Multiply(*this, other, *this);
        return *this;
    }
}

#endif