#include "math/quaternion.hpp"

#if WITH_ISPC
#include "ispc/float_normalize.hpp"
#endif

namespace Engine
{
    const Quat Quat::Identity = Quat(0.0f, 0.0f, 0.0f, 1.0f);

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

    float Quat::Size() const
    {
        return Math::Sqrt(X * X + Y * Y + Z * Z + W * W);
    }

    Quat Quat::Conjugate() const
    {
        return Quat(-X, -Y, -Z, W);
    }

    Quat Quat::Inverse() const
    {
        return Conjugate() / Size();
    }

    Vector3f Quat::RotateVector(const Vector3f& v) const
    {
        const Vector3f q(X, Y, Z);
        const Vector3f t = 2.0f * (q ^ v);
        const Vector3f result = v + (W * t) + (q ^ t);
        return result;
    }

    Vector3f Quat::UnrotateVector(const Vector3f& v) const
    {
        const Vector3f q(-X, -Y, -Z);
        const Vector3f t = 2.0f * (q ^ v);
        const Vector3f result = v + (W * t) + (q ^ t);
        return result;
    }

    Quat Quat::operator*(const Quat& other) const
    {
        return Quat(
            W * other.X + X * other.W + Z * other.Y - Y * other.Z,
            W * other.Y + Y * other.W + X * other.Z - Z * other.X,
            W * other.Z + Z * other.W + Y * other.X - X * other.Y,
            W * other.W - X * other.X - Y * other.Y - Z * other.Z
        );
    }

    Quat Quat::operator*=(const Quat& other)
    {
        X = W * other.X + X * other.W + Z * other.Y - Y * other.Z;
        Y = W * other.Y + Y * other.W + X * other.Z - Z * other.X;
        Z = W * other.Z + Z * other.W + Y * other.X - X * other.Y;
        W = W * other.W - X * other.X - Y * other.Y - Z * other.Z;
        return *this;
    }

    Vector3f Quat::operator*(const Vector3f v) const
    {
        return RotateVector(v);
    }

    Quat Quat::operator*(float scale) const
    {
        return Quat( X * scale, Y * scale, Z * scale, W * scale);
    }

    Quat Quat::operator*=(float scale)
    {
        X *= scale;
        Y *= scale;
        Z *= scale;
        W *= scale;
        return *this;
    }

    Quat Quat::operator/(float scale) const
    {
        const float rscale = 1.0f / scale;
        return Quat( X * rscale, Y * rscale, Z * rscale, W * rscale);
    }

    Quat Quat::operator/=(float scale)
    {
        const float rscale = 1.0f / scale;
        X *= rscale;
        Y *= rscale;
        Z *= rscale;
        W *= rscale;
        return *this;
    }

    Quat Quat::operator+(const Quat& other) const
    {
        return Quat(X + other.X, Y + other.Y, Z + other.Z, W + other.W);
    }

    Quat Quat::operator+=(const Quat& other)
    {
        X += other.X;
        Y += other.Y;
        Z += other.Z;
        W += other.W;
        return *this;
    }

    Quat Quat::operator-(const Quat& other) const
    {
        return Quat(X - other.X, Y - other.Y, Z - other.Z, W - other.W);
    }

    Quat Quat::operator-=(const Quat& other)
    {
        X -= other.X;
        Y -= other.Y;
        Z -= other.Z;
        W -= other.W;
        return *this;
    }

    bool operator==(const Quat& lhs, const Quat& rhs)
    {
        return lhs.Equals(rhs);
    }

    bool operator!=(const Quat& lhs, const Quat& rhs)
    {
        return !lhs.Equals(rhs);
    }

    Quat Quat::Slerp(const Quat& src, const Quat& dest, float slerp)
    {
        // Get cosine of angle between quats.
        const float rawCosine =
                src.X * dest.X +
                src.Y * dest.Y +
                src.Z * dest.Z +
                src.W * dest.W;
        // Unaligned quats - compensate, results in taking shorter route.
        const float cosine = rawCosine >= 0.0f ? rawCosine : -rawCosine;

        float scale0, scale1;

        if(cosine < 0.9999f )
        {
            const float omega = Math::Acos(cosine);
            const float invSin = 1.0f / Math::Sin(omega);
            scale0 = Math::Sin((1.0f - slerp) * omega ) * invSin;
            scale1 = Math::Sin(slerp * omega) * invSin;
        }
        else
        {
            // Use linear interpolation.
            scale0 = 1.0f - slerp;
            scale1 = slerp;
        }

        // In keeping with our flipped cosine:
        scale1 = rawCosine >= 0.0f ? scale1 : -scale1;

        Quat result;
        result.X = scale0 * src.X + scale1 * dest.X;
        result.Y = scale0 * src.Y + scale1 * dest.Y;
        result.Z = scale0 * src.Z + scale1 * dest.Z;
        result.W = scale0 * src.W + scale1 * dest.W;
        result.Normalize();

        return result;
    }
}