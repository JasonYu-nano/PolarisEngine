#pragma once

#include <initializer_list>
#include "definitions_core.hpp"
#include "predefine/platform.hpp"
#include "math/generic_math.hpp"

namespace Engine
{
    template<typename ElementType, typename Allocator>
    class CORE_API List
    {
    public:
        List()
            : MaxSize(AllocatorInstance.GetDefaultCapacity())
        {}

        List(uint64 size)
            : MaxSize(Math::Max(size, AllocatorInstance.GetDefaultCapacity()))
        {}

        List(ElementType* rawPtr, uint64 count) {};

        List(std::initializer_list<ElementType> initList) {};

        List(const List& other) {};

        List(List&& other) {};

        void Add(const ElementType& element) {};

        void Add(ElementType&& element) {};

        void Clear() {};

        ElementType* Data() { return nullptr; }

    private:
        /** make sure AllocatorInstance init first */
        Allocator AllocatorInstance;
        uint64 Size{ 0 };
        uint64 MaxSize{ 0 };
    };
}