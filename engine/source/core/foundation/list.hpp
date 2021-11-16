#pragma once

#include <initializer_list>
#include "definitions_core.hpp"
#include "predefine/platform.hpp"
#include "math/generic_math.hpp"
#include "memory/heap_allocator.hpp"
#include "log/logger.hpp"
#include "foundation/type_traits.hpp"
#include "foundation/functional.hpp"

namespace Engine
{
    template <typename ElementType, typename Allocator = HeapAllocator<uint32>>
    class List
    {
        template <typename T> friend class SparseArray;
    protected:
        typedef typename Allocator::SizeType SizeType;
    public:
        List()
            : Capacity(AllocatorInstance.GetDefaultCapacity())
        {}

        List(SizeType capacity)
            : Capacity(Math::Max(capacity, AllocatorInstance.GetDefaultCapacity()))
        {
            AllocatorInstance.Resize(Capacity, sizeof(ElementType));
        }

        List(ElementType* rawPtr, uint64 count) {};

        List(std::initializer_list<ElementType> initializer) 
        {
            CopyElement(initializer.begin(), (SizeType)initializer.size());
        };

        List(const List& other) 
        {
            CopyElement(other.GetData(), other.Count);
        }

        List(List&& other) noexcept
        {
            MoveElement(Forward<List&&>(other));
        }

        virtual ~List()
        {
            Clear();
        }

        List& operator=(std::initializer_list<ElementType> initializer)
        {
            DestructElements(GetData(), Count);
            CopyElement(initializer.begin(), (SizeType)initializer.size());
            return *this;
        }

        List& operator=(const List& other)
        {
            DestructElements(GetData(), Count);
            CopyElement(other.GetData(), other.Count);
            return *this;
        }

        List& operator=(List&& other) noexcept
        {
            DestructElements(GetData(), Count);
            MoveElement(Forward<List&&>(other));
            return *this;
        }

        ElementType& operator[] (SizeType index)
        {
            BoundCheck();
            PL_ASSERT(index < Count);
            return *(GetData() + index);
        }

        /**
         * add an element at end
         *
         * @param element
         */
        void Add(const ElementType& element)
        {
            AddressCheck(&element);
            EmplaceBack(Forward<const ElementType&>(element));
        };

        /**
         * move an element to end
         *
         * @param element pending move
         */
        void Add(ElementType&& element)
        {
            AddressCheck(&element);
            EmplaceBack(Forward<ElementType&&>(element));
        }

        void Insert(SizeType index, const ElementType& element)
        {
            AddressCheck(&element);
            EmplaceAt(index, element);
        };

        void Insert(SizeType index, ElementType&& element)
        {
            AddressCheck(&element);
            EmplaceAt(index, element);
        };

        void RemoveAt(SizeType index)
        {
            PL_ASSERT(IsValidIndex(index));
            DestructElements(GetData() + index, 1);

            SizeType countToMove = Count - index - 1;
            if (countToMove)
            {
                Memory::Memmove(GetData() + index, GetData() + index + 1, countToMove * sizeof(ElementType));
            }
            Count -= 1;
            //TODO: check need shink
        }

        bool Remove(const ElementType& element)
        {
            RemoveMatch([](const ElementType& inElement) {
                return element == inElement;
            });
        }

        void RemoveMatch(TFunction<bool(const ElementType& element)> predicate)
        {
            if (Count <= 0)
            {
                return;
            }

            SizeType searchIndex = Count - 1;
            while (searchIndex >= 0)
            {
                ElementType* element = GetData() + searchIndex;
                if (predicate(element))
                {
                    RemoveAt(searchIndex);
                }
                searchIndex--;
            }
        }

        void Clear(SizeType slack = 0)
        {
            DestructElements(GetData(), Count);
            Count = 0;
            if (Capacity != slack)
            {
                Resize(slack);
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

        const ElementType* GetData() const
        { 
            if (byte* raw = AllocatorInstance.GetAllocation())
            {
                return (const ElementType*)raw;
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

        bool IsEmpty() const
        {
            return Count == 0;
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
            InsertUnconstructElement(index, 1);
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
            Count += count;
            if (Count > Capacity)
            {
                Expansion();
            }
            return oldCount;
        }

        void InsertUnconstructElement(SizeType index, SizeType count)
        {
            PL_ASSERT(index >= 0 && count > 0 && index <= Count);
            SizeType oldCount = Count;
            Count += count;
            if (Count > Capacity)
            {
                Expansion();
            }

            ElementType* src = GetData() + index;
            Memory::Memmove(src + count, src, (oldCount - index) * sizeof(ElementType));
        }

        void Expansion()
        {
            Capacity = AllocatorInstance.CaculateValidCapacity(Count, Capacity, sizeof(ElementType));
            PL_ASSERT(Count <= Capacity);
            AllocatorInstance.Resize(Capacity, sizeof(ElementType));
        }

        void BoundCheck()
        {
            PL_ASSERT(Count >= 0 && Count <= Capacity);
        }

        void AddressCheck(const ElementType* address)
        {
            PL_ASSERT(address < GetData() || address >= GetData() + Capacity);
        }

        void ConstructElements(ElementType* dest, const ElementType* src, SizeType count)
        {
            Memory::Memcpy((void*)dest, (void*)src, sizeof(ElementType) * count);
        }

        void DestructElements(ElementType* element, SizeType count)
        {
            if constexpr (HasUserDestructorV<ElementType>)
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

        void CopyElement(const ElementType* data, SizeType count)
        {
            PL_ASSERT(data && count > 0 && Count <= 0);
            Count = count;
            Expansion();
            ConstructElements(GetData(), data, count);
        }

        void MoveElement(List&& other)
        {
            Count = other.Count;
            Capacity = other.Capacity;
            AllocatorInstance = MoveTemp(other.AllocatorInstance);
        }

    protected:

        /** make sure AllocatorInstance init first */
        Allocator AllocatorInstance;
        SizeType Count{ 0 };
        SizeType Capacity{ 0 };
    };
}