#include <array>
#include "math/rotator.hpp"
#include "math/quaternion.hpp"
#include "math/matrix.hpp"

#if WITH_ISPC
#include "ispc/rotator_to_quaternion.hpp"
#endif

namespace Engine
{
    bool Rotator::IsZero(float tolerance) const
    {
        return Math::Equals(ClampAxis(Roll), 0.0f, tolerance) &&
                Math::Equals(ClampAxis(Pitch), 0.0f, tolerance) &&
                Math::Equals(ClampAxis(Yaw), 0.0f, tolerance);
    }

    bool Rotator::Equals(const Rotator& other, float tolerance) const
    {
        return Math::Equals(Roll, other.Roll, tolerance) &&
                Math::Equals(Pitch, other.Pitch, tolerance) &&
                Math::Equals(Yaw, other.Yaw, tolerance);
    }

    Vector3f Rotator::ToVector() const
    {
        return Vector3f(Roll, Pitch, Yaw);
    }

    Vector3f Rotator::ToEuler() const
    {
        return Vector3f(ClampAxis(Roll), ClampAxis(Pitch), ClampAxis(Yaw));
    }

    Vector3f Rotator::RotateVector(const Vector3f& v) const
    {
        return Engine::Vector3f();
    }

    Vector3f Rotator::UnrotateVector(const Vector3f& v) const
    {
        return Engine::Vector3f();
    }

    Rotator Rotator::Clamp() const
    {
        return Rotator(ClampAxis(Roll), ClampAxis(Pitch), ClampAxis(Yaw));
    }

    void Rotator::Normalize()
    {
        Roll = NormalizeAxis(Roll);
        Pitch = NormalizeAxis(Pitch);
        Yaw = NormalizeAxis(Yaw);
    }

    Rotator Rotator::GetNormalized() const
    {
        Rotator ret = *this;
        ret.Normalize();
        return ret;
    }

    Rotator Rotator::FromEuler(const Vector3f& euler)
    {
        return Rotator(euler.X, euler.Y, euler.Z);
    }

    float Rotator::ClampAxis(float angle)
    {
        float ret = Math::FMod(angle, 360.0f);
        if (ret < 0)
        {
            ret += 360.f;
        }
        return ret;
    }

    float Rotator::NormalizeAxis(float angle)
    {
        float ret = Math::FMod(angle, 360.0f);
        if (ret > 180.f)
        {
            ret -= 180.f;
        }
        return ret;
    }

    bool operator==(const Rotator& lhs, const Rotator& rhs)
    {
        return lhs.Equals(rhs);
    }

    bool operator!=(const Rotator& lhs, const Rotator& rhs)
    {
        return !lhs.Equals(rhs);
    }

    Quat Rotator::ToQuaternion() const
    {
#if WITH_ISPC
        Quat ret;
        ispc::RotatorToQuat(&Pitch, &ret.X);
        return ret;
#else
        const float degToRad = PI / 180.f;
        const float radDiv2 = degToRad / 2;
        float sp, sy, sr;
        float cp, cy, cr;

        const float pitchNoWinding = Math::FMod(Pitch, 360.0f);
        const float yawNoWinding = Math::FMod(Yaw, 360.0f);
        const float rollNoWinding = Math::FMod(Roll, 360.0f);

        Math::SinCos(pitchNoWinding * radDiv2, sp, cp);
        Math::SinCos(yawNoWinding * radDiv2, sy, cy);
        Math::SinCos(rollNoWinding * radDiv2, sr, cr);

        Quat rotation;
        rotation.X = cr * sp * sy - sr * cp * cy;
        rotation.Y = -cr * sp * cy - sr * cp * sy;
        rotation.Z = cr * cp * sy - sr * sp * cy;
        rotation.W = cr * cp * cy + sr * sp * sy;
        return rotation;
#endif
    }

    Matrix Rotator::ToMatrix() const
    {
        Matrix m;

        float sp, sy, sr;
        float cp, cy, cr;
        Math::SinCos(Math::ToRadians(Pitch), sp, cp);
        Math::SinCos(Math::ToRadians(Yaw), sy, cy);
        Math::SinCos(Math::ToRadians(Roll), sr, cr);

        m.M[0][0] = cp * cy;
        m.M[0][1] = cp * sy;
        m.M[0][2] = sp;
        m.M[0][3] = 0.0f;

        m.M[1][0] = sr * sp * cy - cr * sy;
        m.M[1][1] = sr * sp * sy + cr * cy;
        m.M[1][2] = - sr * cp;
        m.M[1][3] = 0.0f;

        m.M[2][0] = -(cr * sp * cy + sr * sy );
        m.M[2][1] = cy * sr - cr * sp * sy;
        m.M[2][2] = cr * cp;
        m.M[2][3] = 0.0f;

        m.M[3][0] = 0.0f;
        m.M[3][1] = 0.0f;
        m.M[3][2] = 0.0f;
        m.M[3][3] = 1.f;
        return m;
    }
}