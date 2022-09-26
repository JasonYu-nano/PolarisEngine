#pragma once

#include "math/vector.hpp"
#include "definitions_core.hpp"

namespace Engine
{
    struct CORE_API Rotator
    {
        Rotator(float val) : Roll(val), Pitch(val), Yaw(val) {}

        Rotator(float roll, float pitch, float yaw) : Roll(roll), Pitch(pitch), Yaw(yaw) {}

        bool IsZero(float tolerance = KINDA_SMALL_FLOAT) const;

        bool Equals(const Rotator& other, float tolerance = KINDA_SMALL_FLOAT) const;

        Vector3f ToVector() const;

        Vector3f ToEuler() const;

        Vector3f RotateVector(const Vector3f& v) const;

        Vector3f UnrotateVector(const Vector3f& v) const;

        Rotator Clamp() const;

        void Normalize();

        Rotator GetNormalized() const;

        static Rotator FromEuler(const Vector3f& euler);

        static float ClampAxis( float angle );

        static float NormalizeAxis( float angle );

        friend bool operator== (const Rotator& lhs, const Rotator& rhs);

        friend bool operator!= (const Rotator& lhs, const Rotator& rhs);

        Rotator operator+ (const Rotator& other);

        Rotator operator- (const Rotator& other);

        static const Rotator Zero;

        float Roll;
        float Pitch;
        float Yaw;
    };
}