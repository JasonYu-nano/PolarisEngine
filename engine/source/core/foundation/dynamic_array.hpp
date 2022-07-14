#pragma once

#include <initializer_list>
#include "definitions_core.hpp"
#include "predefine/platform.hpp"
#include "math/generic_math.hpp"
#include "memory/allocator_policies.hpp"
#include "log/logger.hpp"
#include "foundation/type_traits.hpp"
#include "foundation/functional.hpp"

namespace Engine
{
#pragma region iterator
    template <typename ContainerType, typename ElementType, typename SizeType>
    class ConstIndexIterator
    {
    public:
        ConstIndexIterator(const ContainerType& container, SizeType index)
            : Container(container)
            , Index(index)
        {}

        const ElementType& operator* () const
        {
            return Container[Index];
        }

        const ElementType* operator-> () const
        {
            return &Container[Index];
        }

        explicit operator bool () const
        {
            return Index >= 0 && Index < Container.Size();
        }

        ConstIndexIterator& operator++ ()
        {
            ++Index;
            return *this;
        }

        ConstIndexIterator& operator-- ()
        {
            --Index;
            return *this;
        }

        SizeType GetIndex() const
        {
            return Index;
        }

        friend bool operator== (const ConstIndexIterator& lhs, const ConstIndexIterator& rhs)
        {
            return (&lhs.Container == &rhs.Container) && (lhs.Index == rhs.Index);
        }

        friend bool operator!= (const ConstIndexIterator& lhs, const ConstIndexIterator& rhs)
        {
            return !(lhs == rhs);
        }

    protected:
        const ContainerType& Container;
        SizeType Index;
    };

    template <typename ContainerType, typename ElementType, typename SizeType>
    class IndexIterator : public ConstIndexIterator<ContainerType, ElementType, SizeType>
    {
        using Super = ConstIndexIterator<ContainerType, ElementType, SizeType>;

    public:
        IndexIterator(const ContainerType& container, SizeType index)
            : Super(container, index)
        {}

        ElementType& operator* () const
        {
            return const_cast<ElementType&>(this->Container[this->Index]);
        }

        ElementType* operator-> () const
        {
            return const_cast<ElementType*>(&(this->Container[this->Index]));
        }

        IndexIterator& operator++ ()
        {
            Super::operator++();
            return *this;
        }

        IndexIterator& operator-- ()
        {
            Super::operator--();
            return *this;
        }

        void RemoveSelf()
        {
            const_cast<ContainerType&>(this->Container).RemoveAt(this->Index);
            this->Index--;
        }

        friend bool operator== (const IndexIterator& lhs, const IndexIterator& rhs)
        {
            return (&lhs.Container == &rhs.Container) && (lhs.Index == rhs.Index);
        }

        friend bool operator!= (const IndexIterator& lhs, const IndexIterator& rhs)
        {
            return !(lhs == rhs);
        }
    };
#pragma endregion iterator

    template <typename ElementType, typename Allocator = DefaultAllocator>
    class DynamicArray
    {
        template <typename T, typename U> friend class SparseArray;

    protected:
        using AllocatorType = typename Allocator::template ElementAllocator<ElementType>;
        using SizeType = typename AllocatorType::SizeType;

    public:
        using ConstIterator = ConstIndexIterator<DynamicArray, ElementType, SizeType>;
        using Iterator = IndexIterator<DynamicArray, ElementType, SizeType>;
        using TElement = ElementType;

    public:
        DynamicArray()
            : ArrayCapacity(AllocatorInstance.GetDefaultSize())
        {}

        explicit DynamicArray(SizeType capacity)
            : ArrayCapacity(Math::Max(capacity, AllocatorInstance.GetDefaultSize()))
        {
            AllocatorInstance.Resize(ArrayCapacity);
        }

        DynamicArray(const ElementType* rawPtr, SizeType count)
        {
            CopyElement(rawPtr, count);
        }

        DynamicArray(const ElementType& initVal, SizeType count)
        {
            Resize(count, initVal);
        }

        DynamicArray(std::initializer_list<ElementType> initializer) 
        {
            CopyElement(initializer.begin(), (SizeType)initializer.size());
        };

        DynamicArray(const DynamicArray& other)
        {
            CopyElement(other.Data(), other.ArraySize);
        }

        DynamicArray(DynamicArray&& other) noexcept
        {
            MoveElement(Forward<DynamicArray&&>(other));
        }

        template <typename OtherAllocator>
        explicit DynamicArray(const DynamicArray<ElementType, OtherAllocator>& other)
        {
            CopyElement(other.Data(), other.Size());
        }

        virtual ~DynamicArray()
        {
            Clear();
        }

        DynamicArray& operator=(std::initializer_list<ElementType> initializer)
        {
            DestructElements(Data(), ArraySize);
            CopyElement(initializer.begin(), (SizeType)initializer.size());
            return *this;
        }

        DynamicArray& operator=(const DynamicArray& other)
        {
            PL_ASSERT(this != &other);
            DestructElements(Data(), ArraySize);
            CopyElement(other.Data(), other.ArraySize);
            return *this;
        }

        DynamicArray& operator=(DynamicArray&& other) noexcept
        {
            PL_ASSERT(this != &other);
            DestructElements(Data(), ArraySize);
            MoveElement(Forward<DynamicArray&&>(other));
            return *this;
        }

        template <typename OtherAllocator>
        DynamicArray& operator=(const DynamicArray<ElementType, OtherAllocator>& other)
        {
            DestructElements(Data(), ArraySize);
            CopyElement(other.Data(), (SizeType) other.Size());
            return *this;
        }

        bool operator== (DynamicArray other) const
        {
            if (ArraySize != other.ArraySize)
            {
                return false;
            }

            const ElementType* selfPtr = Data();
            const ElementType* otherPtr = other.Data();
            for (SizeType i = 0; i < ArraySize; ++i)
            {
                if (selfPtr[i] != otherPtr[i])
                {
                    return false;
                }
            }

            return true;
        }

        bool operator!= (DynamicArray other) const
        {
            return !(*this == other);
        }

        ElementType& operator[] (SizeType index)
        {
            BoundCheck();
            PL_ASSERT(index < ArraySize);
            return *(Data() + index);
        }

        const ElementType& operator[] (SizeType index) const
        {
            BoundCheck();
            PL_ASSERT(index < ArraySize);
            return *(Data() + index);
        }

        /**
         * Add an element at end
         * @param element
         */
        void Add(const ElementType& element)
        {
            AddressCheck(&element);
            EmplaceBack(Forward<const ElementType&>(element));
        };

        /**
         * Move an element to end
         * @param element
         */
        void Add(ElementType&& element)
        {
            AddressCheck(&element);
            EmplaceBack(Forward<ElementType&&>(element));
        }

        void Add(const ElementType* elements, SizeType count)
        {
            SizeType index = AddUnconstructElement(count);
            ConstructElements(Data() + index, elements, count);
        }

        /**
         * Add an element use default constuctor, return the reference of element.
         * @return Reference of element
         */
        ElementType& AddDefault()
        {
            EmplaceBack();
            return Data()[ArraySize - 1];
        }

        /**
         * Inset element at specifier position
         * @param index
         * @param element
         */
        void Insert(SizeType index, const ElementType& element)
        {
            PL_ASSERT(0 <=index && index <= ArraySize);
            AddressCheck(&element);
            EmplaceAt(index, element);
        };

        /**
         * Inset element at specifier position
         * @param index
         * @param element
         */
        void Insert(SizeType index, ElementType&& element)
        {
            PL_ASSERT(0 <=index && index <= ArraySize);
            AddressCheck(&element);
            EmplaceAt(index, element);
        };

        void Insert(SizeType index, const ElementType* elements, SizeType size)
        {
            PL_ASSERT(0 <=index && index <= ArraySize && size > 0);
            AddressCheck(elements);
            InsertUnconstructElement(index, size);
            ConstructElements(Data() + index, elements, size);
        };

        /**
         * Remove element at position
         * @param index
         */
        void RemoveAt(SizeType index)
        {
            PL_ASSERT(IsValidIndex(index));
            DestructElements(Data() + index, 1);

            SizeType countToMove = ArraySize - index - 1;
            if (countToMove)
            {
                Memory::Memmove(Data() + index, Data() + index + 1, countToMove * sizeof(ElementType));
            }
            ArraySize -= 1;
            //TODO: check need shink
        }

        /**
         * Remove all elements equals param
         * @param element
         * @return has element been removed
         */
        bool Remove(const ElementType& element)
        {
            return RemoveMatch([&element](const ElementType& inElement) {
                return element == inElement;
            }) > 0;
        }

        void Remove(SizeType first, SizeType last)
        {
            PL_ASSERT(IsValidIndex(first) && IsValidIndex(last));
            if (first <= last)
            {
                SizeType removeCount = last - first + 1;
                DestructElements(Data() + first, removeCount);
                SizeType countToMove = ArraySize - last - 1;
                if (countToMove)
                {
                    Memory::Memmove(Data() + first, Data() + last + 1, countToMove * sizeof(ElementType));
                }
                ArraySize -= removeCount;
            }
        }

        /**
         * Remove all elements match the predicate
         * @param predicate
         * @return count of elements been removed
         */
        SizeType RemoveMatch(TFunction<bool(const ElementType& element)> predicate)
        {
            if (ArraySize <= 0)
            {
                return 0;
            }

            SizeType searchCount = ArraySize;
            SizeType matchCount = 0;
            while (searchCount > 0)
            {
                ElementType* element = Data() + searchCount - 1;
                if (predicate(*element))
                {
                    RemoveAt(searchCount - 1);
                    matchCount++;
                }
                searchCount--;
            }

            return matchCount;
        }

        /**
         * Remove all elements
         * @param slack remain capacity
         */
        void Clear(SizeType slack = 0)
        {
            DestructElements(Data(), ArraySize);
            ArraySize = 0;
            if (ArrayCapacity != slack)
            {
                Reserve(slack);
            }
        }

        ElementType& At(SizeType index)
        {
            BoundCheck();
            if (IsValidIndex(index))
            {
                return Data() + index;
            }
            throw std::out_of_range{"Index an element out of range"};
        }

        ElementType* Data()
        {
            if (byte* raw = AllocatorInstance.GetAllocation())
            {
                return (ElementType*)raw;
            }
            return nullptr;
        }

        const ElementType* Data() const
        { 
            if (byte* raw = AllocatorInstance.GetAllocation())
            {
                return (const ElementType*)raw;
            }
            return nullptr;
        }

        SizeType Size() const
        {
            return ArraySize;
        }

        SizeType Capacity() const
        {
            return ArrayCapacity;
        }

        constexpr SizeType MaxSize() const
        {
            return NumericLimits<SizeType>::Max();
        }

        bool IsEmpty() const
        {
            return ArraySize == 0;
        }

        template <typename... Args>
        void Resize(SizeType count, Args&&... args)
        {
            PL_ASSERT(count >= 0);
            if (count < ArraySize)
            {
                DestructElements(Data() + count, ArraySize - count);
            }
            else if (count > ArraySize)
            {
                Reserve(count);
                for (SizeType idx = ArraySize; idx < count; ++idx)
                {
                    new(Data() + idx) ElementType(Forward<Args>(args)...);
                }
            }
            ArraySize = count;
        }

        void Reserve(SizeType newCapacity)
        {
            if (newCapacity > ArraySize && newCapacity != ArrayCapacity)
            {
                ArrayCapacity = newCapacity;
                AllocatorInstance.Resize(newCapacity);
            }
        }

        bool IsValidIndex(SizeType index) const
        {
            return index < ArraySize;
        }


        Iterator begin()
        {
            return Iterator(*this, 0);
        }

        ConstIterator begin() const
        {
            return ConstIterator(*this, 0);
        }

        Iterator end()
        {
            return Iterator(*this, ArraySize);
        }

        ConstIterator end() const
        {
            return ConstIterator(*this, ArraySize);
        }

        Iterator rbegin()
        {
            return Iterator(*this, ArraySize - 1);
        }

        ConstIterator rbegin() const
        {
            return ConstIterator(*this, ArraySize - 1);
        }

        Iterator rend()
        {
            return Iterator(*this, (SizeType)-1);
        }

        ConstIterator rend() const
        {
            return ConstIterator(*this, (SizeType)-1);
        }

        ConstIterator cbegin()
        {
            return ConstIterator(*this, 0);
        }

        ConstIterator cend()
        {
            return ConstIterator(*this, ArraySize - 1);
        }

        ConstIterator crbegin()
        {
            return ConstIterator(*this, 0);
        }

        ConstIterator crend()
        {
            return ConstIterator(*this, (SizeType)-1);
        }
    private:
        template <typename... Args>
        void EmplaceBack(Args&&... args)
        {
            SizeType index = AddUnconstructElement(1);
            new(Data() + index) ElementType(Forward<Args>(args)...);
        }

        template <typename... Args>
        void EmplaceAt(SizeType index, Args&&... args)
        {
            InsertUnconstructElement(index, 1);
            new(Data() + index) ElementType(Forward<Args>(args)...);
        }

        /**
         * Add given count of unconstruct element.
         *
         * @param count count of add elements
         * @return start index of add elements
         */
        SizeType AddUnconstructElement(SizeType count)
        {
            PL_ASSERT(count > 0);
            SizeType oldCount = ArraySize;
            ArraySize += count;
            if (ArraySize > ArrayCapacity)
            {
                Expansion();
            }
            return oldCount;
        }

        void InsertUnconstructElement(SizeType index, SizeType count)
        {
            PL_ASSERT(index >= 0 && count > 0 && index <= ArraySize);
            SizeType oldCount = ArraySize;
            ArraySize += count;
            if (ArraySize > ArrayCapacity)
            {
                Expansion();
            }

            ElementType* src = Data() + index;
            Memory::Memmove(src + count, src, (oldCount - index) * sizeof(ElementType));
        }

        void Expansion()
        {
            ArrayCapacity = CalculateGrowth(ArraySize);
            PL_ASSERT(ArraySize <= ArrayCapacity);
            AllocatorInstance.Resize(ArrayCapacity);
        }

        SizeType CalculateGrowth(const SizeType newSize) const
        {
            SizeType oldCapacity = Capacity();
            SizeType max = MaxSize();

            if (oldCapacity > max - oldCapacity / 2)
            {
                return max;
            }

            const SizeType geometric = oldCapacity + oldCapacity / 2;

            if (geometric < newSize)
            {
                return newSize;
            }

            return geometric;
        }

        void BoundCheck() const
        {
            PL_ASSERT(ArraySize >= 0 && ArraySize <= ArrayCapacity);
        }

        void AddressCheck(const ElementType* address)
        {
            PL_ASSERT(address < Data() || address >= Data() + ArrayCapacity);
        }

        void ConstructElements(ElementType* dest, const ElementType* src, SizeType count)
        {
            Memory::Memcpy((void*)dest, (void*)src, sizeof(ElementType) * count);
        }

        void DestructElements(ElementType* element, SizeType count)
        {
            if constexpr (!std::is_union<ElementType>::value)
            {
                while (count)
                {
                    std::destroy_at(element);
                    ++element;
                    --count;
                }
            }
        }

        void CopyElement(const ElementType* data, SizeType count)
        {
            PL_ASSERT(data && count > 0);
            ArraySize = count;
            if (ArraySize > ArrayCapacity)
            {
                Expansion();
            }
            ConstructElements(Data(), data, count);
        }

        void MoveElement(DynamicArray&& other)
        {
            AllocatorInstance = MoveTemp(other.AllocatorInstance);
            ArraySize = other.ArraySize;
            ArrayCapacity = other.ArrayCapacity;
            other.ArraySize = 0;
            other.ArrayCapacity = 0;
        }

    protected:

        /** Make sure AllocatorInstance init first */
        AllocatorType AllocatorInstance;
        SizeType ArraySize{ 0 };
        SizeType ArrayCapacity{ 0 };
    };

    template <typename ElementType>
    using DynamicArray64 = DynamicArray<ElementType, HeapSizeAllocator<int64>>;
}