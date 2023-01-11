#pragma once

#include <bit>
#include "definitions_core.hpp"
#include "foundation/type_traits.hpp"
#include <cmath>

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

        static uint32 FloorLog2(uint32 value)
        {
            // see http://codinggorilla.domemtech.com/?p=81 or http://en.wikipedia.org/wiki/Binary_logarithm but modified to return 0 for a input value of 0
            // 686ms on test data
            uint32 pos = 0;
            if (value >= 1 << 16) { value >>= 16; pos += 16; }
            if (value >= 1 << 8) { value >>= 8; pos += 8; }
            if (value >= 1 << 4) { value >>= 4; pos += 4; }
            if (value >= 1 << 2) { value >>= 2; pos += 2; }
            if (value >= 1 << 1) { pos += 1; }
            return (value == 0) ? 0 : pos;
        }

        static uint32 CountLeadingZeros(uint32 value)
        {
            if (value == 0) return 32;
            return 31 - FloorLog2(value);
        }

        static uint32 CeilLogTwo(uint32 value)
        {
            int32 bitMask = ((int32)(CountLeadingZeros(value) << 26)) >> 31;
            return (32 - CountLeadingZeros(value - 1)) & (~bitMask);
        }

        template <IntegralType T>
        static constexpr bool IsPowerOfTwo(std::make_unsigned_t<T> value)
        {
            return value != 0 && (value & (value - 1)) == 0;
        }

        template <IntegralType T>
        static T RoundUpToPowerOfTwo(T value) {
            if(IsPowerOfTwo<T>(value))
            {
                return value;
            }

            if(value == 0)
            {
                return 1;
            }

            --value;
            for(uint64 i = 1; i < sizeof(uint64) * CHAR_BIT; i *= 2)
            {
                value |= value >> i;
            }

            return value + 1;
        }

//        static uint32 RoundUpToPowerOfTwo(uint32 value)
//        {
//            return 1 << CeilLogTwo(value);
//        }

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

        constexpr static inline uint32 CountTrailingZeroBits(uint32 v) noexcept
        {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
            return std::countr_zero(v);
#else
    return QtPrivate::qConstexprCountTrailingZeroBits(v);
#endif
        }
    };

}