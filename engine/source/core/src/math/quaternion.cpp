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
        return Quat();
    }
}