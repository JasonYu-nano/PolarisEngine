#pragma once
#include "core/predefine/platform.hpp"
#include "core/foundation/type_traits.hpp"

namespace Engine
{
    template<typename T>
    T Align(T value, uint64 aligment)
    {
        static_assert(IsIntegral<T>::value || IsPointer<T>::value, "Align expects an integer or pointer type");

        return (T)(((uint64)value + aligment - 1) & ~(aligment - 1));
    }
}