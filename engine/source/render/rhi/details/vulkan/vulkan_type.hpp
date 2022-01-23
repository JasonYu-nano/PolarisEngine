#pragma once
#include "foundation/dynamic_array.hpp"

namespace Engine
{
    template <typename ElementType>
    using VkArray = DynamicArray<ElementType, HeapAllocator<uint32>>;
}