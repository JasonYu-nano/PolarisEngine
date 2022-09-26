#include "math/rotator.hpp"

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
}