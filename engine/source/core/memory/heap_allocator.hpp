#pragma once
#include "foundation/type_traits.hpp"

namespace Engine
{
    template<typename UIntType, EnableIfT<IsIntegral<typename UIntType>::value,bool> = true>
    class HeapAllocator
    {
        static_assert()
    };
}