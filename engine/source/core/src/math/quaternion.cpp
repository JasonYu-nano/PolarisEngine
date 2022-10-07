#include "math/quaternion.hpp"

#if WITH_ISPC
#include "ispc/float_normalize.hpp"
#endif

namespace Engine
{
    const Quat Quat::Identity = Quat(0.0f);

    Quat::Quat(const Rotator& rotator)
    {
        *this = rotator.ToQuaternion();
    }

    bool Quat::Equals(const Quat& other, float tolerance) const
    {
        return Math::Equals(X, other.X, tolerance) &&
                Math::Equals(Y, other.Y, tolerance) &&
                Math::Equals(Z, other.Z, tolerance) &&
                Math::Equals(W, other.W, tolerance);
    }

    void Quat::Normalize()
    {
#if WITH_ISPC
        ispc::FloatNormalizeFast(&X, 4);
#else
        float sizeSq = X * X + Y * Y + Z * Z + W * W;
        float scale = 1.0f / Math::Sqrt(sizeSq);
        X *= scale;
        Y *= scale;
        Z *= scale;
        W *= scale;
#endif
    }

    bool operator==(const Quat& lhs, const Quat& rhs)
    {
        return lhs.Equals(rhs);
    }

    bool operator!=(const Quat& lhs, const Quat& rhs)
    {
        return !lhs.Equals(rhs);
    }
}