#pragma once
#include "foundation/dynamic_array.hpp"

namespace Engine
{
    template <typename ElementType, typename Allocator = HeapAllocator<uint32>>
    class Stack : public DynamicArray<ElementType, Allocator>
    {
        typedef DynamicArray<ElementType, Allocator> Super;
    public:
        Stack() : Super() {};

        Stack(Super::SizeType capacity) : Super(capacity) {};

        ~Stack()
        {
            Super::~DynamicArray();
        }

        void Push(const ElementType& element)
        {
            EmplaceBack(element);
        }

        void Push(ElementType&& element)
        {
            EmplaceBack(element);
        }

        void Pop()
        {
            PL_ASSERT(Super::Count > 0);
            RemoveAt(Super::Count - 1);
        }

        ElementType& Top()
        {
            PL_ASSERT(Super::Count > 0);
            return At(Super::Count - 1);
        }
    private:
        using Super::Add;
        using Super::Insert;
        using Super::RemoveAt;
        using Super::Remove;
        using Super::RemoveMatch;
        using Super::At;
    };
}