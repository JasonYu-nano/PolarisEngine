#include <array>
#include "math/rotator.hpp"
#include "math/quaternion.hpp"

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
        std::array<float, 4> components {0.0f};
        ispc::RotatorToQuat(&Pitch, components.data());
        return Quat(components);
#else
        const float degToRad = PI / 180.f;
        const float radDiv2 = degToRad / 2;
        float sp, sy, sr;
        float cp, cy, cr;

        const float pitchNoWinding = Math::FMod(Pitch, 360.0f);
        const float yawNoWinding = Math::FMod(Yaw, 360.0f);
        const float rollNoWinding = Math::FMod(Roll, 360.0f);

        sp = Math::Sin(pitchNoWinding * radDiv2);
        sy = Math::Sin(yawNoWinding * radDiv2);
        sr = Math::Sin(rollNoWinding * radDiv2);
        cp = Math::Cos(pitchNoWinding * radDiv2);
        cy = Math::Cos(yawNoWinding * radDiv2);
        cr = Math::Cos(rollNoWinding * radDiv2);

        Quat rotation;
        rotation.X = cr * sp * sy - sr * cp * cy;
        rotation.Y = -cr * sp * cy - sr * cp * sy;
        rotation.Z = cr * cp * sy - sr * sp * cy;
        rotation.W = cr * cp * cy + sr * sp * sy;
        return rotation;
#endif
    }
}