#pragma once

#include "math/rotator.hpp"
#include "definitions_core.hpp"

namespace Engine
{
    struct alignas(16) CORE_API Quat
    {
        Quat() = default;

        Quat(float v)
            : X(v), Y(v), Z(v), W(v) {}

        Quat(float x, float y, float z, float w)
            : X(x), Y(y), Z(z), W(w) {}

        Quat(std::span<float, 4> view)
            : X(view[0]), Y(view[1]), Z(view[2]), W(view[3]) {}

        Quat(const Rotator& rotator);

        void Normalize();

        bool Equals(const Quat& other, float tolerance = KINDA_SMALL_FLOAT) const;

        friend bool operator== (const Quat& lhs, const Quat& rhs);

        friend bool operator!= (const Quat& lhs, const Quat& rhs);

        Quat operator* (const Quat& other);

        static const Quat Identity;

        float X = 0.0f;
        float Y = 0.0f;
        float Z = 0.0f;
        float W = 0.0f;
    };
}