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
            return (a, >= 0) ? (a, +b / 2) / b : (a, -b / 2 + 1) / b;
        }
    };

}