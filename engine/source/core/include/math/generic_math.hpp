#pragma once

#include <bit>
#include <cmath>
#include "definitions_core.hpp"
#include "foundation/type_traits.hpp"

namespace Engine
{
    constexpr float PI = 3.1415926535897932f;
    constexpr float INV_PI = 0.31830988618f;
    constexpr float HALF_PI = 1.57079632679f;

    class CORE_API Math
    {
    public:
        Math() = delete;

        template <typename T>
        static T Max(const T a, const T b)
        {
            return (a >= b) ? a : b;
        }

        template <typename T>
        static T Min(const T a, const T b)
        {
            return (a <= b) ? a : b;
        }

        template <typename T>
        static T Abs(const T val)
        {
            return std::abs(val);
        }

        template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
        static float Sqrt(const T val)
        {
            return std::sqrt(static_cast<float>(val));
        }

        static float Sqrt(const float val)
        {
            return std::sqrt(val);
        }

        static double Sqrt(const double val)
        {
            return std::sqrt(val);
        }

        static float FMod(float a, float b)
        {
            return std::fmodf(a, b);
        }

        template <typename T>
        static bool Equals(const T lhs, const T rhs, float tolerance)
        {
            return Abs(lhs - rhs) <= tolerance;
        }

        static float ToRadians(float degrees)
        {
            return FMod(degrees, 360.0f) * PI / 180.0f;
        }

        static float Sin(float radians)
        {
            return std::sin(radians);
        }

        static float Cos(float radians)
        {
            return std::cos(radians);
        }

        static void SinCos(float value, float& sin, float& cos);

        static float Acos(float cos)
        {
            return std::acos(cos);
        }

        template <typename T>
        static T DivideAndRoundNearest(T a, T b)
        {
            return (a >= 0) ? (a + b / 2) / b : (a - b / 2 + 1) / b;
        }

        template <typename T>
        static T DivideAndCeil(T a, T b)
        {
            return (a + b - 1) / b;
        }

        template <typename T>
        static T CeilToMultiple(T a, T multiple)
        {
            if (a == 0)
            {
                return 0;
            }

            return ((a - 1) / multiple + 1) * multiple;
        }

        template <UnsignedIntegralType T>
        static constexpr T FloorLogTwo(T x)
        {
            // For x > 0, FloorLogTwo(y) returns floor(log(x)/log(2)).

            int y = 0;

            while (x > 1)
            {
                y +=  1;
                x >>= 1;
            }

            return y;
        }

        template <UnsignedIntegralType T>
        static constexpr T CeilLogTwo(T x)
        {
            // For x > 0, CeilLogTwo(y) returns ceil(log(x)/log(2)).
            int y = 0;
            int r = 0;

            while (x > 1)
            {
                if (x & 1)
                    r = 1;

                y +=  1;
                x >>= 1;
            }

            return y + r;
        }

        template <typename T>
        static constexpr T CountLeadingZeros(T val)
        {
            return std::countl_zero(val);
        }

        template <typename T>
        static constexpr T CountTrailingZeros(T val)
        {
            return std::countr_zero(val);
        }

        template <IntegralType T>
        static constexpr bool IsPowerOfTwo(std::make_unsigned_t<T> value)
        {
            return value != 0 && (value & (value - 1)) == 0;
        }

        template <IntegralType T>
        static T RoundUpToPowerOfTwo(T value)
        {
            if (IsPowerOfTwo<T>(value))
            {
                return value;
            }

            if (value == 0)
            {
                return 1;
            }

            --value;
            for (uint64 i = 1; i < sizeof(uint64) * CHAR_BIT; i *= 2)
            {
                value |= value >> i;
            }

            return value + 1;
        }

        template <typename T>
        static T Clamp(const T& value, const T& min, const T& max)
        {
            if (value > max)
            {
                return max;
            }
            else if (value < min)
            {
                return min;
            }

            return value;
        }
    };
}