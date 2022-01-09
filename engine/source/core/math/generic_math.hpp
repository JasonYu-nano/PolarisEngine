#pragma once

#include "definitions_core.hpp"

namespace Engine
{
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

        static uint32 RoundUpToPowerOfTwo(uint32 value)
        {
            return 1 << CeilLogTwo(value);
        }
    };

}