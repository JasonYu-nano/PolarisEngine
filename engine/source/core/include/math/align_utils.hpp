#pragma once
#include "global.hpp"
#include "foundation/type_traits.hpp"

namespace Engine
{
    template<typename T>
    T Align(T value, uint64 aligment)
    {
        static_assert(std::is_integral_v<T> || std::is_pointer_v<T>, "Align expects an integer or pointer type");

        return (T)(((uint64)value + aligment - 1) & ~(aligment - 1));
    }
}