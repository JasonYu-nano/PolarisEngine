#pragma once

#include <initializer_list>
#include "definitions_core.hpp"
#include "math/generic_math.hpp"
#include "memory/allocator_policies.hpp"
#include "foundation/type_traits.hpp"
#include "foundation/functional.hpp"
#include "foundation/details/compressed_pair.hpp"

namespace Engine
{
    template <typename ContainerType, typename ElementType, typename SizeType>
    class ConstArrayIterator
    {
    public:
        ConstArrayIterator(const ContainerType& container, SizeType index)
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

        ConstArrayIterator& operator++ ()
        {
            ++Index;
            return *this;
        }

        ConstArrayIterator operator++ (int32)
        {
            ConstArrayIterator temp = *this;
            ++*this;
            return temp;
        }

        ConstArrayIterator& operator-- ()
        {
            --Index;
            return *this;
        }

        ConstArrayIterator operator-- (int32)
        {
            ConstArrayIterator temp = *this;
            --*this;
            return temp;
        }

        SizeType GetIndex() const
        {
            return Index;
        }

        friend bool operator== (const ConstArrayIterator& lhs, const ConstArrayIterator& rhs)
        {
            return (&lhs.Container == &rhs.Container) && (lhs.Index == rhs.Index);
        }

        friend bool operator!= (const ConstArrayIterator& lhs, const ConstArrayIterator& rhs)
        {
            return !(lhs == rhs);
        }

    protected:
        const ContainerType& Container;
        SizeType Index;
    };

    template <typename ContainerType, typename ElementType, typename SizeType>
    class ArrayIterator : public ConstArrayIterator<ContainerType, ElementType, SizeType>
    {
        using Super = ConstArrayIterator<ContainerType, ElementType, SizeType>;

    public:
        ArrayIterator(const ContainerType& container, SizeType index)
                : Super(container, index)
        {}

        ElementType& operator* () const
        {
            return const_cast<ElementType&>(Super::operator*());
        }

        ElementType* operator-> () const
        {
            return const_cast<ElementType*>(Super::operator->());
        }

        ArrayIterator& operator++ ()
        {
            Super::operator++();
            return *this;
        }

        ArrayIterator operator++ (int32)
        {
            ArrayIterator temp = *this;
            Super::operator++();
            return temp;
        }

        ArrayIterator& operator-- ()
        {
            Super::operator--();
            return *this;
        }

        ArrayIterator operator-- (int32)
        {
            ArrayIterator temp = *this;
            Super::operator--();
            return temp;
        }

        void RemoveSelf()
        {
            const_cast<ContainerType&>(this->Container).RemoveAt(this->Index);
            this->Index--;
        }

        friend bool operator== (const ArrayIterator& lhs, const ArrayIterator& rhs)
        {
            return (&lhs.Container == &rhs.Container) && (lhs.Index == rhs.Index);
        }

        friend bool operator!= (const ArrayIterator& lhs, const ArrayIterator& rhs)
        {
            return !(lhs == rhs);
        }
    };

    template<typename ValueType, typename SizeType, typename Pointer>
    class ArrayVal
    {
    public:
        ArrayVal() = default;
        ~ArrayVal() = default;

        Pointer Data{ nullptr };
        SizeType Size{ 0 };
        SizeType Capacity{ 0 };
    };

    template <typename Elem, typename Alloc = StandardAllocator<int32>>
    class Array
    {
        template <typename T, typename U> friend class SparseArray;

    public:
        using AllocatorType = typename Alloc::template ElementAllocator<Elem>;
        using ValueType = typename AllocatorType::ValueType;
        using SizeType = typename AllocatorType::SizeType;
        using ConstIterator = ConstArrayIterator<Array, Elem, SizeType>;
        using Iterator = ArrayIterator<Array, Elem, SizeType>;

    private:
        using SecondaryVal = ArrayVal<ValueType, SizeType, ValueType*>;

    public:
        Array() = default;

        explicit Array(SizeType capacity)
        {
            Reserve(capacity);
        }

        Array(const ValueType* ptr, SizeType size)
        {
            CopyElement(ptr, size);
        }

        Array(const ValueType& initVal, SizeType count)
        {
            Resize(count, initVal);
        }

        Array(std::initializer_list<ValueType> initializer)
        {
            CopyElement(initializer.begin(), static_cast<SizeType>(initializer.size()));
        };

        Array(const Array& other)
        {
            auto& otherVal = other.Pair.Second();
            CopyElement(otherVal.Data, otherVal.Size);
        }

        Array(Array&& other) noexcept
        {
            MoveElement(std::forward<Array>(other));
        }

        Array(const Array& other, SizeType extraSlack)
        {
            auto& otherVal = other.Pair.Second();
            CopyElement(otherVal.Data, otherVal.Size, extraSlack);
        }

        template <typename OtherAllocator>
        explicit Array(const Array<ValueType, OtherAllocator>& other)
        {
            auto& otherVal = other.Pair.Second();
            CopyElement(otherVal.Data, otherVal.Size);
        }

        virtual ~Array()
        {
            Clear();
        }

        Array& operator=(std::initializer_list<ValueType> initializer)
        {
            CopyElement(initializer.begin(), static_cast<SizeType>(initializer.size()));
            return *this;
        }

        Array& operator=(const Array& other)
        {
            ENSURE(this != &other);
            auto& otherVal = other.Pair.Second();
            CopyElement(otherVal.Data, otherVal.Size);
            return *this;
        }

        Array& operator=(Array&& other) noexcept
        {
            ENSURE(this != &other);
            MoveElement(std::forward<Array>(other));
            return *this;
        }

        template <typename OtherAllocator>
        Array& operator=(const Array<Elem, OtherAllocator>& other)
        {
            auto& otherVal = other.Pair.Second();
            CopyElement(otherVal.Data, otherVal.Size);
            return *this;
        }

        bool operator== (const Array& other) const
        {
            auto& myVal = Pair.Second();
            auto& otherVal = other.Pair.Second();
            if (myVal.Size != otherVal.Size)
            {
                return false;
            }

            for (SizeType i = 0; i < myVal.Size; ++i)
            {
                if (myVal.Data[i] != otherVal.Data[i])
                {
                    return false;
                }
            }

            return true;
        }

        bool operator!= (const Array& other) const
        {
            return !(*this == other);
        }

        ValueType& operator[] (SizeType index)
        {
            ENSURE(BoundCheck() && IsValidIndex(index));
            return *(Data() + index);
        }

        const ValueType& operator[] (SizeType index) const
        {
            ENSURE(BoundCheck() && IsValidIndex(index));
            return *(Data() + index);
        }

        /**
         * Add an element at end
         * @param elem
         */
        void Add(const ValueType& elem)
        {
            ENSURE(AddressCheck(&elem));
            EmplaceBack(elem);
        };

        /**
         * Move an element to end
         * @param elem
         */
        void Add(ValueType&& elem)
        {
            ENSURE(AddressCheck(&elem));
            EmplaceBack(std::forward<ValueType>(elem));
        }

        void Add(const ValueType* elems, SizeType size)
        {
            SizeType index = AddUnconstructElement(size);
            ConstructElements(Data() + index, elems, size);
        }

        /**
         * Add an element use default constuctor, return the reference of element.
         * @return Reference of element
         */
        ValueType& AddDefault()
        {
            EmplaceBack();
            return Data()[Size() - 1];
        }

        bool AddUnique(const ValueType& elem)
        {
            bool isUnique = true;
            auto& myVal = Pair.Second();

            for (SizeType idx = 0; idx < myVal.Size; ++idx)
            {
                if (myVal.Data[idx] == elem)
                {
                    isUnique = false;
                    break;
                }
            }

            if (isUnique)
            {
                Add(elem);
            }

            return isUnique;
        }

        bool AddUnique(ValueType&& elem)
        {
            bool isUnique = true;
            auto& myVal = Pair.Second();

            for (SizeType idx = 0; idx < myVal.Size; ++idx)
            {
                if (myVal.Data[idx] == elem)
                {
                    isUnique = false;
                    break;
                }
            }

            if (isUnique)
            {
                Add(std::forward<ValueType>(elem));
            }

            return isUnique;
        }

        void Push(const ValueType& elem)
        {
            Add(elem);
        }

        void Push(ValueType&& elem)
        {
            Add(std::forward<ValueType>(elem));
        }

        /**
         * Inset element at specifier position
         * @param index
         * @param elem
         */
        void Insert(SizeType index, const ValueType& elem)
        {
            ENSURE(AddressCheck(&elem) && 0 <=index && index <= Size());
            EmplaceAt(index, elem);
        };

        /**
         * Inset element at specifier position
         * @param index
         * @param elem
         */
        void Insert(SizeType index, ValueType&& elem)
        {
            ENSURE(AddressCheck(&elem) && 0 <=index && index <= Size());
            EmplaceAt(index, std::forward<ValueType>(elem));
        };

        void Insert(SizeType index, const ValueType* elems, SizeType size)
        {
            auto& myVal = Pair.Second();
            ENSURE(AddressCheck(elems) && 0 <= index && index <= myVal.Size && size > 0);
            InsertUnconstructElement(index, size);
            ConstructElements(myVal.Data + index, elems, size);
        };

        /**
         * Remove element at position
         * @param index
         */
        void RemoveAt(SizeType index)
        {
            ENSURE(IsValidIndex(index));
            auto& myVal = Pair.Second();
            DestructElements(myVal.Data + index, 1);

            SizeType countToMove = myVal.Size - index - 1;
            if (countToMove)
            {
                Memory::Memmove(myVal.Data + index, myVal.Data + index + 1, countToMove * sizeof(ValueType));
            }
            myVal.Size -= 1;
            //TODO: check need shink
        }

        void RemoveAtSwap(SizeType index)
        {
            ENSURE(IsValidIndex(index));
            auto& myVal = Pair.Second();
            DestructElements(myVal.Data + index, 1);
            myVal.Size -= 1;
            if (index != myVal.Size)
            {
                Memory::Memmove(myVal.Size + index, myVal.Data + myVal.Size, sizeof(ValueType));
            }
        }

        /**
         * Remove all elements equals param
         * @param element
         * @return has element been removed
         */
        bool Remove(const Elem& element)
        {
            return RemoveMatch([&element](const Elem& inElement) {
                return element == inElement;
            }) > 0;
        }

        void Remove(SizeType first, SizeType last)
        {
            ENSURE(IsValidIndex(first) && IsValidIndex(last));
            if (first <= last)
            {
                auto& myVal = Pair.Second();
                SizeType removeCount = last - first + 1;
                DestructElements(Data() + first, removeCount);
                SizeType countToMove = myVal.Size - last - 1;
                if (countToMove)
                {
                    Memory::Memmove(myVal.Data + first, myVal.Data + last + 1, countToMove * sizeof(ValueType));
                }
                myVal.Size -= removeCount;
            }
        }

        /**
         * Remove all elements match the predicate
         * @param predicate
         * @return count of elements been removed
         */
        SizeType RemoveMatch(std::function<bool(const Elem& element)> predicate)
        {
            auto& myVal = Pair.Second();
            if (myVal.Size <= 0)
            {
                return 0;
            }

            SizeType searchCount = myVal.Size;
            SizeType matchCount = 0;
            while (searchCount > 0)
            {
                ValueType* element = Data() + searchCount - 1;
                if (predicate(*element))
                {
                    RemoveAt(searchCount - 1);
                    matchCount++;
                }
                searchCount--;
            }

            return matchCount;
        }

        ValueType Pop()
        {
            auto& myVal = Pair.Second();
            ENSURE(myVal.Size > 0);
            ValueType last = At(myVal.Size - 1);
            RemoveAt(myVal.Size - 1);
            return last;
        }

        /**
         * Remove all elements
         * @param slack remain capacity
         */
        void Clear(SizeType slack = 0)
        {
            auto& myVal = Pair.Second();
            DestructElements(myVal.Data, myVal.Size);
            myVal.Size = 0;
            if (myVal.Capacity != slack)
            {
                Reserve(slack);
            }
        }

        Elem& At(SizeType index)
        {
            ENSURE(BoundCheck() && IsValidIndex(index));
            return *(Data() + index);
        }

        Elem* Data()
        {
            return Pair.Second().Data;
        }

        const Elem* Data() const
        {
            return Pair.Second().Data;
        }

        SizeType Size() const
        {
            return Pair.Second().Size;
        }

        SizeType Capacity() const
        {
            return Pair.Second().Capacity;
        }

        constexpr SizeType MaxSize() const
        {
            return NumericLimits<SizeType>::Max();
        }

        bool Empty() const
        {
            return Size() == 0;
        }

        ValueType& Last()
        {
            auto& myVal = Pair.Second();
            ENSURE(myVal.Size > 0);
            return myVal.Data[myVal.Size - 1];
        }

        const ValueType& Last() const
        {
            auto& myVal = Pair.Second();
            ENSURE(myVal.Size > 0);
            return myVal.Data[myVal.Size - 1];
        }

        ValueType& Top()
        {
            return Last();
        }

        const ValueType& Top() const
        {
            return Last();
        }

        AllocatorType GetAllocator() const
        {
            return static_cast<AllocatorType>(GetAlloc());
        }

        void Resize(SizeType newSize)
        {
            ENSURE(newSize >= 0);
            auto& myVal = Pair.Second();
            if (newSize < myVal.Size)
            {
                DestructElements(myVal.Data + newSize, myVal.Size - newSize);
            }
            else if (newSize > myVal.Size)
            {
                Reserve(newSize);
                for (SizeType idx = myVal.Size; idx < newSize; ++idx)
                {
                    new(myVal.Data + idx) ValueType();
                }
            }
            myVal.Size = newSize;
        }

        void Resize(SizeType newSize, const ValueType& val)
        {
            ENSURE(newSize >= 0);
            auto& myVal = Pair.Second();
            if (newSize < myVal.Size)
            {
                DestructElements(myVal.Data + newSize, myVal.Size - newSize);
            }
            else if (newSize > myVal.Size)
            {
                Reserve(newSize);
                for (SizeType idx = myVal.Size; idx < newSize; ++idx)
                {
                    ConstructElements(myVal.Data + idx, &val, 1);
                }
            }
            myVal.Size = newSize;
        }

        void Reserve(SizeType newCapacity)
        {
            auto& myVal = Pair.Second();
            if (newCapacity > myVal.Capacity)
            {
                ENSURE(newCapacity <= MaxSize());
                Reallocate(newCapacity);
            }
        }

        bool IsValidIndex(SizeType index) const
        {
            return index < Size();
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
            return Iterator(*this, Size());
        }

        ConstIterator end() const
        {
            return ConstIterator(*this, Size());
        }

        Iterator rbegin()
        {
            return Iterator(*this, Size() - 1);
        }

        ConstIterator rbegin() const
        {
            return ConstIterator(*this, Size() - 1);
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
            return ConstIterator(*this, Size() - 1);
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
            new(Data() + index) ValueType(std::forward<Args>(args)...);
        }

        template <typename... Args>
        void EmplaceAt(SizeType index, Args&&... args)
        {
            InsertUnconstructElement(index, 1);
            new(Data() + index) ValueType(std::forward<Args>(args)...);
        }

        /**
         * Add given count of unconstruct element.
         *
         * @param count count of add elements
         * @return start index of add elements
         */
        SizeType AddUnconstructElement(SizeType count)
        {
            ENSURE(count > 0);
            auto& myVal = Pair.Second();
            SizeType oldSize = myVal.Size;
            myVal.Size += count;
            if (myVal.Size > myVal.Capacity)
            {
                Expansion(myVal.Size);
            }
            return oldSize;
        }

        void InsertUnconstructElement(SizeType index, SizeType count)
        {
            auto& myVal = Pair.Second();
            ENSURE(index >= 0 && count > 0 && index <= myVal.Size);
            SizeType oldSize = myVal.Size;
            myVal.Size += count;
            if (myVal.Size > myVal.Capacity)
            {
                Expansion(myVal.Size);
            }

            ValueType* src = myVal.Data + index;
            Memory::Memmove(src + count, src, (oldSize - index) * sizeof(ValueType));
        }

        void Expansion(SizeType destSize = -1)
        {
            destSize = destSize >= 0 ? destSize : Pair.Second().Size;
            SizeType newCapacity = CalculateGrowth(destSize);
            ENSURE(destSize <= newCapacity);
            Reallocate(newCapacity);
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

        bool BoundCheck() const
        {
            const auto& myVal = Pair.Second();
            return myVal.Size >= 0 && myVal.Size <= myVal.Capacity;
        }

        bool AddressCheck(const ValueType* address) const
        {
            const auto& myVal = Pair.Second();
            return address < myVal.Data || address >= myVal.Data + myVal.Capacity;
        }

        void ConstructElements(ValueType* dest, const ValueType* src, SizeType size)
        {
            if constexpr (CAN_MEMORY_COPY)
            {
                Memory::Memcpy((void*)dest, (void*)src, sizeof(ValueType) * size);
            }
            else
            {
                while (size > 0)
                {
                    new(dest) ValueType(*src);
                    ++dest;
                    ++src;
                    --size;
                }
            }
        }

        void DestructElements(ValueType* elems, SizeType size)
        {
            while (size)
            {
                std::destroy_at(elems);
                ++elems;
                --size;
            }
        }

        void CopyElement(const ValueType* data, SizeType size, SizeType extraSlack = 0)
        {
            ENSURE(data && size > 0 && extraSlack >= 0);
            auto& myVal = Pair.Second();
            if (myVal.Data)
            {
                DestructElements(myVal.Data, myVal.Size);
            }

            myVal.Size = size;
            if (myVal.Size + extraSlack > myVal.Capacity)
            {
                Expansion(myVal.Size + extraSlack);
            }
            ConstructElements(myVal.Data, data, size);
        }

        void MoveElement(Array&& other)
        {
            auto& myVal = Pair.Second();
            auto& otherVal = other.Pair.Second();

            if (myVal.Data)
            {
                DestructElements(myVal.Data, myVal.Size);
                const auto& alloc = GetAlloc();
                alloc.Deallocate(myVal.Data, myVal.Capacity);
            }

            myVal.Size = otherVal.Size;
            myVal.Capacity = otherVal.Capacity;
            myVal.Data = otherVal.Data;

            otherVal.Data = nullptr;
            otherVal.Size = 0;
            otherVal.Capacity = 0;
        }

        void Reallocate(SizeType newCapacity)
        {
            auto& myVal = Pair.Second();
            auto& alloc = Pair.First();

            ValueType* newPtr = alloc.Allocate(newCapacity);
            if (myVal.Data)
            {
                Memory::Memmove(newPtr, myVal.Data, myVal.Size * sizeof(ValueType));
                alloc.Deallocate(myVal.Data, myVal.Capacity);
            }

            myVal.Data = newPtr;
            myVal.Capacity = newCapacity;
        }

        AllocatorType& GetAlloc()
        {
            return Pair.First();
        }

        const AllocatorType& GetAlloc() const
        {
            return Pair.First();
        }

    protected:
        static constexpr bool CAN_MEMORY_COPY = std::is_trivially_copyable_v<ValueType>;

        CompressedPair<AllocatorType, SecondaryVal> Pair;
    };
}