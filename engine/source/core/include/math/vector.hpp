#pragma once

#include "global.hpp"
#include "memory/memory.hpp"
#include "math/generic_math.hpp"
#include <span>

namespace Engine
{
    constexpr float VECTOR_NORMALIZED_THRESHOLD = 0.001f;

    constexpr float VECTOR_EQUALS_DEFAULT_TOLERANCE = 0.0001f;

    template <typename T, uint32 N>
    struct Vector
    {
        Vector(std::span<T, N> view)
        {
            Memory::Memcpy(Data, view.data(), N);
        }

        T Size() const
        {
            return static_cast<T>(Math::Sqrt(SizeSquared()));
        }

        T SizeSquared() const
        {
            T ret = 0;
            for (uint32 i = 0; i < N; i++)
            {
                ret += Data[i] * Data[i];
            }
            return ret;
        }

        bool IsZero(float tolerance = 0.0f) const
        {
            return SizeSquared() <= tolerance;
        }

        T operator[] (int32 idx) const
        {
            return Data[idx];
        }

        T& operator[] (int32 idx)
        {
            return Data[idx];
        }

        friend bool operator== (const Vector& lhs ,const Vector& rhs)
        {
            for (uint32 i = 0; i < N; ++i)
            {
                if (!Math::Equals(lhs[i], rhs[i], VECTOR_EQUALS_DEFAULT_TOLERANCE))
                {
                    return false;
                }
            }
            return true;
        }

        friend bool operator!= (const Vector& lhs ,const Vector& rhs)
        {
            for (uint32 i = 0; i < N; ++i)
            {
                if (!Math::Equals(lhs[i], rhs[i], VECTOR_EQUALS_DEFAULT_TOLERANCE))
                {
                    return true;
                }
            }
            return false;
        }

        T Data[N] = { 0 };
    };

    template <typename T>
    class Vector<T, 3>
    {
    public:
        Vector() = default;

        Vector(const Vector& other) = default;

        Vector(float val) : X(val), Y(val), Z(val) {}

        Vector(float x, float y, float z) : X(x), Y(y), Z(z) {}

        Vector(std::span<T, 3> view)
        {
            Memory::Memcpy(Data, view.data(), 3);
        }

        static Vector Cross(const Vector& lhs, const Vector& rhs)
        {
            return lhs ^ rhs;
        }

        static T Dot(const Vector& lhs, const Vector& rhs)
        {
            return lhs | rhs;
        }

        static const Vector Zero;

        static const Vector XAxis;

        static const Vector YAxis;

        static const Vector ZAxis;

        T Size() const
        {
            return Math::Sqrt(SizeSquared());
        }

        T SizeSquared() const
        {
            return X * X + Y * Y + Z * Z;
        }

        T Size2D() const
        {
            return Math::Sqrt(SizeSquared2D());
        }

        T SizeSquared2D() const
        {
            return X * X + Y * Y;
        }

        void Normalize()
        {
            EXPECT(SizeSquared() > static_cast<T>(0));
            float scale = 1.0f / Size();
            X *= scale;
            Y *= scale;
            Z *= scale;
        }

        bool IsNormalized() const
        {
            return (Math::Abs(1.0f - SizeSquared()) < VECTOR_NORMALIZED_THRESHOLD);
        }

        bool IsZero(float tolerance = 0.0f) const
        {
            return (X == 0 && Y == 0 && Z == 0) || SizeSquared() < tolerance;
        }

        bool IsUnit() const
        {
            return (X == static_cast<T>(1) && Y == static_cast<T>(0) && Z == static_cast<T>(0)) ||
                    (X == static_cast<T>(0) && Y == static_cast<T>(1) && Z == static_cast<T>(0)) ||
                    (X == static_cast<T>(0) && Y == static_cast<T>(0) && Z == static_cast<T>(1));
        }

        float operator[] (int32 idx) const
        {
            return Data[idx];
        }

        float& operator[] (int32 idx)
        {
            return Data[idx];
        }

        Vector& operator= (const Vector& other)
        {
            Data = other.Data;
            return *this;
        }

        friend bool operator== (const Vector& lhs, const Vector& rhs)
        {
            return Math::Equals(lhs.X, rhs.X, VECTOR_EQUALS_DEFAULT_TOLERANCE) &&
                    Math::Equals(lhs.Y, rhs.Y, VECTOR_EQUALS_DEFAULT_TOLERANCE) &&
                    Math::Equals(lhs.Z, rhs.Z, VECTOR_EQUALS_DEFAULT_TOLERANCE);
        }

        friend bool operator!= (const Vector& lhs, const Vector& rhs)
        {
            return !Math::Equals(lhs.X, rhs.X, VECTOR_EQUALS_DEFAULT_TOLERANCE) ||
                    !Math::Equals(lhs.Y, rhs.Y, VECTOR_EQUALS_DEFAULT_TOLERANCE) ||
                    !Math::Equals(lhs.Z, rhs.Z, VECTOR_EQUALS_DEFAULT_TOLERANCE);
        }

        /**
         * Cross product
         * @param other
         * @return
         */
        Vector operator^ (const Vector& other)
        {
            return Vector(
                Y * other.Z - Z * other.Y,
                Z * other.X - X * other.Z,
                X * other.Y - Y * other.X);
        }

        /**
         * Dot product
         * @param other
         * @return
         */
        T operator| (const Vector& other)
        {
            return X * other.X + Y * other.Y + Z * other.Z;
        }

        Vector operator* (T scale) const
        {
            return Vector(X * scale, Y * scale, Z * scale);
        }

        Vector operator*= (T scale) const
        {
            *this = *this * scale;
            return *this;
        }

        Vector operator* (const Vector& other) const
        {
            return Vector(X * other.X, Y * other.Y, Z * other.Z);
        }

        Vector operator*= (const Vector& other) const
        {
            *this = *this * other;
            return *this;
        }

        Vector operator/ (T scale) const
        {
            T inverseScale = 1.0f / scale;
            return Vector(X * inverseScale, Y * inverseScale, Z * inverseScale);
        }

        Vector operator/= (T scale) const
        {
            *this = *this / scale;
            return *this;
        }

        Vector operator/ (const Vector& other) const
        {
            return Vector(X / other.X, Y / other.Y, Z / other.Z);
        }

        Vector operator/= (const Vector& other) const
        {
            *this = *this / other;
            return *this;
        }

        union
        {
            struct
            {
                T X;
                T Y;
                T Z;
            };

            T Data[3];
        };
    };

    template <typename T>
    const Vector<T, 3> Vector<T, 3>::Zero = Vector<T, 3>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));

    template <typename T>
    const Vector<T, 3> Vector<T, 3>::XAxis = Vector<T, 3>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));

    template <typename T>
    const Vector<T, 3> Vector<T, 3>::YAxis = Vector<T, 3>(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0));

    template <typename T>
    const Vector<T, 3> Vector<T, 3>::ZAxis = Vector<T, 3>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));

    using Vector3f = Vector<float, 3>;
}