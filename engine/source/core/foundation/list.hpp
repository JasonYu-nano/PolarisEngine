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
        typedef typename Allocator::SizeType SizeType;
    public:
        List()
            : Capacity(AllocatorInstance.GetDefaultCapacity())
        {}

        List(SizeType capacity)
            : Capacity(Math::Max(capacity, AllocatorInstance.GetDefaultCapacity()))
        {
            AllocatorInstance.Resize(Capacity);
        }

        List(ElementType* rawPtr, uint64 count) {};

        List(std::initializer_list<ElementType> initList) {};

        List(const List& other) {};

        List(List&& other) {};

        ElementType& operator[] (SizeType index)
        {
            BoundCheck();
            return GetData() + index;
        }

        /**
         * add an element at end
         *
         * @param element
         */
        void Add(const ElementType& element)
        {
            EmplaceBack(Forward<const ElementType&>(element));
        };

        /**
         * move an element to end
         *
         * @param element pending move
         */
        void Add(ElementType&& element)
        {
            EmplaceBack(Forward<ElementType&&>(element));
        }

        void Insert(SizeType index, const ElementType& element)
        {
            EmplaceAt(index, element);
        };

        void Insert(SizeType index, ElementType&& element)
        {
            EmplaceAt(index, element);
        };

        void Clear(SizeType slack)
        {
            DestructElements(GetData(), Count);
            Count = 0;
            if (Capacity != slack)
            {
                Resize();
            }
        }

        ElementType& At(SizeType index)
        {
            BoundCheck();
            if (IsValidIndex(index))
            {
                return GetData() + index;
            }
            throw std::out_of_range{};
        }

        ElementType* GetData() 
        { 
            if (byte* raw = AllocatorInstance.GetAllocation())
            {
                return (ElementType*)raw;
            }
            return nullptr;
        }

        SizeType GetCount() const
        {
            return Count;
        }

        SizeType GetCapacity() const
        {
            return Capacity;
        }

        void Resize(SizeType newCapacity)
        {
            if (newCapacity > Count && newCapacity != Capacity)
            {
                Capacity = newCapacity;
                AllocatorInstance.Resize(newCapacity, sizeof(ElementType));
            }
        }

        bool IsValidIndex(SizeType index) const
        {
            return index < Count;
        }
    private:
        template <typename... Args>
        void EmplaceBack(Args&&... args)
        {
            SizeType index = AddUnconstructElement(1);
            new(GetData() + index) ElementType(Forward<Args>(args)...);
        }

        template <typename... Args>
        void EmplaceAt(SizeType index, Args&&... args)
        {
            InserUnconstructElement(index, 1);
            new(GetData() + index) ElementType(Forward<Args>(args)...);
        }

        /**
         * add given count of unconstruct element
         * 
         * @param count count of add elements
         * @return start index of add elements
         */
        SizeType AddUnconstructElement(SizeType count)
        {
            PL_ASSERT(count > 0);
            SizeType oldCount = Count;
            if (oldCount + count > Capacity)
            {
                Expansion(oldCount + count);
            }
            return oldCount;
        }

        void InserUnconstructElement(SizeType index, SizeType count)
        {
            PL_ASSERT(index >= 0 && count > 0 && index <= Count);
            SizeType oldCount = Count;
            if (oldCount + count > Capacity)
            {
                Expansion(oldCount + count);
            }

            ElementType* src = GetData() + index;
            Memory::Memmove(src + count, src, oldCount - index);
        }

        void Expansion(SizeType newSize)
        {
            Capacity = AllocatorInstance.CaculateValidCapacity(newSize, Capacity, sizeof(ElementType));
            PL_ASSERT(newSize <= Capacity);
            Count = newSize;
            AllocatorInstance.Resize(Capacity, sizeof(ElementType));
        }

        void BoundCheck()
        {
            PL_ASSERT(Count >= 0 && Count <= Capacity);
        }

        void DestructElements(ElementType* element, SizeType count)
        {
            if constexpr (HasDestructorV<ElementType>)
            {
                while (count)
                {
                    typedef ElementType DestructItemsElementTypeTypedef;

                    element->DestructItemsElementTypeTypedef::~DestructItemsElementTypeTypedef();
                    ++element;
                    --count;
                }
            }
        }

        /** make sure AllocatorInstance init first */
        Allocator AllocatorInstance;
        SizeType Count{ 0 };
        SizeType Capacity{ 0 };
    };
}