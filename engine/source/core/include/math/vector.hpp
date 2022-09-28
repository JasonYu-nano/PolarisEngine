#pragma once

#include "global.hpp"
#include "memory/memory.hpp"
#include "math/generic_math.hpp"
#include <span>

#if WITH_ISPC
#include "ispc/cross_product.hpp"
#endif

namespace Engine
{
    constexpr float VECTOR_NORMALIZED_THRESH = 0.001f;

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
                if (!Math::Equals(lhs[i], rhs[i], KINDA_SMALL_FLOAT))
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
                if (!Math::Equals(lhs[i], rhs[i], KINDA_SMALL_FLOAT))
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

        Vector GetNormalized() const
        {
            Vector ret = *this;
            ret.Normalize();
            return ret;
        }

        bool IsNormalized() const
        {
            return (Math::Abs(1.0f - SizeSquared()) < VECTOR_NORMALIZED_THRESH);
        }

        bool IsZero(float tolerance = 0.0f) const
        {
            return (X == 0 && Y == 0 && Z == 0) || SizeSquared() < tolerance;
        }

        bool Equals(const Vector& other, float tolerance = KINDA_SMALL_FLOAT)
        {
            return Math::Equals(X, other.X, tolerance) &&
                   Math::Equals(Y, other.Y, tolerance) &&
                   Math::Equals(Z, other.Z, tolerance);
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
            return Math::Equals(lhs.X, rhs.X, KINDA_SMALL_FLOAT) &&
                    Math::Equals(lhs.Y, rhs.Y, KINDA_SMALL_FLOAT) &&
                    Math::Equals(lhs.Z, rhs.Z, KINDA_SMALL_FLOAT);
        }

        friend bool operator!= (const Vector& lhs, const Vector& rhs)
        {
            return !Math::Equals(lhs.X, rhs.X, KINDA_SMALL_FLOAT) ||
                    !Math::Equals(lhs.Y, rhs.Y, KINDA_SMALL_FLOAT) ||
                    !Math::Equals(lhs.Z, rhs.Z, KINDA_SMALL_FLOAT);
        }

        /**
         * Cross product
         * @param other
         * @return
         */
        Vector operator^ (const Vector& other) const
        {
#if WITH_ISPC
            Vector ret;
            ispc::CrossProduct(Data, other.Data, ret.Data);
            return ret;
#else
            return Vector(
                Y * other.Z - Z * other.Y,
                Z * other.X - X * other.Z,
                X * other.Y - Y * other.X);
#endif
        }

        /**
         * Dot product
         * @param other
         * @return
         */
        T operator| (const Vector& other) const
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