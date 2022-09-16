#pragma once

#include "global.hpp"

namespace Engine
{
    class Vector
    {
    public:
        Vector(float val) : X(val), Y(val), Z(val) {}

        Vector(float x, float y, float z) : X(x), Y(y), Z(z) {}

        static Vector Cross(const Vector& lhs, const Vector& rhs);

        static float Dot(const Vector& lhs, const Vector& rhs);

        float Size() const;

        float SizeSquared() const;

        float Size2D() const;

        float SizeSquared2D() const;

        void Normalize();

        bool IsNormalized() const;

        bool IsZero() const;

        bool IsUnit() const;

        float operator[] (int32 idx) const;

        float& operator[] (int32 idx);

        float X{ 0.0f };
        float Y{ 0.0f };
        float Z{ 0.0f };
    };
}