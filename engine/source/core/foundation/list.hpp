#pragma once

#include <initializer_list>
#include "definitions_core.hpp"
#include "predefine/platform.hpp"
#include "math/generic_math.hpp"
#include "memory/heap_allocator.hpp"
#include "log/logger.hpp"
#include "foundation/type_traits.hpp"

namespace Engine
{
    template <typename ElementType, typename Allocator = HeapAllocator<uint64>>
    class CORE_API List
    {
        typedef Allocator::SizeType SizeType;
    public:
        List()
            : Capacity(AllocatorInstance.GetDefaultCapacity())
        {}

        List(SizeType size)
            : Capacity(Math::Max(size, AllocatorInstance.GetDefaultCapacity()))
        {
            AllocatorInstance.Resize(Capacity);
        }

        List(ElementType* rawPtr, uint64 count) {};

        List(std::initializer_list<ElementType> initList) {};

        List(const List& other) {};

        List(List&& other) {};

        void Add(const ElementType& element)
        {
            EmplaceBack(Forward<const ElementType&>(element));
        };

        void Add(ElementType&& element)
        {
            EmplaceBack(Forward<ElementType&&>(element));
        }

        void Clear() {};

        ElementType* GetData() 
        { 
            if (byte* raw = AllocatorInstance.GetAllocation())
            {
                return (ElementType*)raw;
            }
            return nullptr;
        }

    private:
        template <typename... Args>
        void EmplaceBack(Args&&... args)
        {
            SizeType index = AddSize(1);
            new(GetData() + index) ElementType(Forward<Args>(args)...);
        }

        /**
         * add given count of unconstruct element size memory
         * 
         * @param count count of add elements
         * @return start index of add elements
         */
        SizeType AddSize(SizeType count)
        {
            PL_ASSERT(count > 0);
            SizeType oldSize = Size;
            if (oldSize + count > Capacity)
            {
                //TODO: resize
            }
            return oldSize;
        }

        /** make sure AllocatorInstance init first */
        Allocator AllocatorInstance;
        SizeType Size{ 0 };
        SizeType Capacity{ 0 };
    };
}