#pragma once

#include "foundation/bit_array.hpp"
#include "foundation/array.hpp"
#include "foundation/dynamic_array.hpp"
#include "foundation/type_traits.hpp"

namespace Engine
{
    template <typename ContainerType>
    class ConstSparseIterator
    {
        using ValueType = typename ContainerType::ValueType;
        using SizeType = typename ContainerType::SizeType;
        using BitArrayType = typename ContainerType::BitArrayType;
        using BitArrayIter = typename BitArrayType::ConstValidIterator;
    public:
        ConstSparseIterator(const ContainerType& container, BitArrayIter bitIter)
            : Container(container)
            , BitIterator(bitIter)
        {}

        const ValueType& operator*() const { return Container[GetIndex()]; }

        const ValueType* operator->() const { return &Container[GetIndex()]; }

        ConstSparseIterator& operator++ ()
        {
            ++BitIterator;
            return *this;
        }

        SizeType GetIndex() const { return BitIterator.GetIndex(); }

        friend bool operator== (const ConstSparseIterator& lhs, const ConstSparseIterator& rhs)
        {
            return (&lhs.Container == &rhs.Container) && (lhs.BitIterator == rhs.BitIterator);
        }

        friend bool operator!= (const ConstSparseIterator& lhs, const ConstSparseIterator& rhs)
        {
            return !(lhs == rhs);
        }

    protected:
        const ContainerType& Container;
        BitArrayIter BitIterator;
    };

    template <typename ContainerType>
    class SparseIterator : public ConstSparseIterator<ContainerType>
    {
        using Super = ConstSparseIterator<ContainerType>;
        using ValueType = typename ContainerType::ValueType;
        using SizeType = typename ContainerType::SizeType;
        using BitArrayType = typename ContainerType::BitArrayType;
        using BitArrayIter = typename BitArrayType::ConstValidIterator;
    public:
        SparseIterator(ContainerType& container, BitArrayIter bitIter)
            : Super(container, bitIter)
        {}

        ValueType& operator*() const { return const_cast<ValueType&>(Super::operator*()); }

        ValueType* operator->() const { return const_cast<ValueType*>(Super::operator->()); }

        SparseIterator& operator++ ()
        {
            Super::operator++();
            return *this;
        }

        SizeType GetIndex() const { return Super::GetIndex(); }

        friend bool operator== (const SparseIterator& lhs, const SparseIterator& rhs)
        {
            return (&lhs.Container == &rhs.Container) && (lhs.BitIterator == rhs.BitIterator);
        }

        friend bool operator!= (const SparseIterator& lhs, const SparseIterator& rhs)
        {
            return !(lhs == rhs);
        }
    };

    template <typename Elem, typename Alloc = StandardAllocator<int32>>
    class SparseArray
    {
    public:
        using ValueType = Elem;
        using SizeType = typename Alloc::SizeType;

        union _SparseArrayLinkNode
        {
            _SparseArrayLinkNode() {};
            ~_SparseArrayLinkNode() {};

            ValueType Val;
            struct
            {
                SizeType PrevIndex;
                SizeType NextIndex;
            };
        };

        using NodeType = _SparseArrayLinkNode;
        using AllocatorType = typename Alloc::template ElementAllocator<NodeType>;
        using ArrayType = Array<NodeType, Alloc>;
        using BitArrayType = BitArray<StandardAllocator<SizeType>>;
        using ConstIterator = ConstSparseIterator<SparseArray>;
        using Iterator = SparseIterator<SparseArray>;

        explicit SparseArray(SizeType capacity, const AllocatorType& alloc = AllocatorType())
            : AllocateFlags(capacity)
            , ElemNodes(capacity, alloc)
        {}

        SparseArray(ValueType* ptr, SizeType size, const AllocatorType& alloc = AllocatorType())
            : AllocateFlags()
            , ElemNodes(alloc)
        {
            CopyElement(ptr, size);
        }

        SparseArray(std::initializer_list<ValueType> initializer, const AllocatorType& alloc = AllocatorType())
            : AllocateFlags()
            , ElemNodes(alloc)
        {
            CopyElement(initializer.begin(), static_cast<SizeType>(initializer.size()));
        }

        SparseArray(const SparseArray& other)
            : FirstFreeNodeIndex(other.FirstFreeNodeIndex)
            , FreeElemCount(other.FreeElemCount)
            , AllocateFlags(other.AllocateFlags)
            , ElemNodes(other.ElemNodes)
        {}

        SparseArray(SparseArray&& other) noexcept
            : FirstFreeNodeIndex(other.FirstFreeNodeIndex)
            , FreeElemCount(other.FreeElemCount)
            , AllocateFlags(std::move(other.AllocateFlags))
            , ElemNodes(std::move(other.ElemNodes))
        {
            other.FirstFreeNodeIndex = INDEX_NONE;
            other.FreeElemCount = 0;
        }

        template <typename OtherAllocator>
        explicit SparseArray(const SparseArray<ValueType, OtherAllocator>& other)
            : FirstFreeNodeIndex(other.FirstFreeNodeIndex)
            , FreeElemCount(other.FreeElemCount)
            , AllocateFlags(other.AllocateFlags)
            , ElemNodes(other.ElemNodes)
        {}

        ~SparseArray()
        {
            Clear();
        }

        SparseArray& operator= (std::initializer_list<ValueType> initializer)
        {
            Clear(initializer.size());
            CopyElement(initializer.begin(), static_cast<SizeType>(initializer.size()));
        }

        SparseArray& operator= (const SparseArray& other)
        {
            ENSURE(this != &other);
            FirstFreeNodeIndex = other.FirstFreeNodeIndex;
            FreeElemCount = other.FreeElemCount;
            AllocateFlags = other.AllocateFlags;
            ElemNodes = other.ElemNodes;
            return *this;
        }

        SparseArray& operator= (SparseArray&& other) noexcept
        {
            ENSURE(this != &other);
            FirstFreeNodeIndex = other.FirstFreeNodeIndex;
            other.FirstFreeNodeIndex = INDEX_NONE;
            FreeElemCount = other.FreeElemCount;
            other.FreeElemCount = 0;
            AllocateFlags = std::move(other.AllocateFlags);
            ElemNodes = std::move(other.ElemNodes);
            return *this;
        }

        template <typename OtherAllocator>
        SparseArray& operator= (const SparseArray<ValueType, OtherAllocator>& other)
        {
            FirstFreeNodeIndex = other.FirstFreeNodeIndex;
            FreeElemCount = other.FreeElemCount;
            AllocateFlags = other.AllocateFlags;
            ElemNodes = other.ElemNodes;
            return *this;
        }

        bool operator== (const SparseArray& other) const
        {
            if (MaxIndex() != other.MaxIndex())
            {
                return false;
            }

            for (SizeType idx = 0; idx < MaxIndex(); ++idx)
            {
                if (HasValue(idx) != other.HasValue(idx))
                {
                    return false;
                }

                if (HasValue(idx) && (*this)[idx] != other[idx])
                {
                    return false;
                }
            }
            return true;
        }

        bool operator!= (const SparseArray& other) const
        {
            return !(*this == other);
        }

        ValueType& operator[] (SizeType index)
        {
            ENSURE(0 <=index && AllocateFlags[index]);
            return ElemNodes[index].Val;
        }

        const ValueType& operator[] (SizeType index) const
        {
            ENSURE(0 <= index && AllocateFlags[index]);
            return ElemNodes[index].Val;
        }

        SizeType Add(const ValueType& elem)
        {
            return Emplace(elem);
        }

        SizeType Add(ValueType&& elem)
        {
            return Emplace(std::move(elem));
        }

        void Insert(SizeType index, const ValueType& element)
        {
            InsertUnconstructElement(index);
            new(GetData() + index) ValueType(element);
        }

        void Insert(SizeType index, ValueType&& element)
        {
            InsertUnconstructElement(index);
            new(GetData() + index) ValueType(element);
        }

        void RemoveAt(SizeType index)
        {
            ENSURE(0 <= index && index < MaxIndex() && AllocateFlags[index] == true);
            NodeType& node = ElemNodes[index];
            std::destroy_at(&node.Val);
            RemoveWithoutDestruct(index, &node);
        }

        void Clear(SizeType slack = 0)
        {
            for (Iterator iter = begin(); iter != end(); ++iter)
            {
                std::destroy_at(&(*iter));
            }

            FirstFreeNodeIndex = INDEX_NONE;
            FreeElemCount = 0;
            ElemNodes.Clear(slack);
            AllocateFlags.Clear(slack);
        }

        void Resize(SizeType newSize)
        {
            Resize(newSize, ValueType());
        }

        void Resize(SizeType newSize, const ValueType& val)
        {
            SizeType mySize = Size();
            if (newSize == mySize)
            {
                return;
            }

            if (newSize < mySize)
            {
                SizeType curSize = 0;
                for (Iterator it = begin(); it != end(); ++it)
                {
                    ++curSize;
                    if (curSize > newSize)
                    {
                        RemoveAt(it.GetIndex());
                    }
                }
                return;
            }

            SizeType elemToAdd = newSize - mySize;
            while (elemToAdd > 0)
            {
                --elemToAdd;
                Emplace(val);
            }
        }

        void Reserve(SizeType capacity)
        {
            SizeType maxIndex = MaxIndex();
            if (maxIndex >= capacity)
            {
                return;
            }

            SizeType elemToAdd = capacity - maxIndex;
            SizeType startIndex = ElemNodes.AddUnconstructElement(elemToAdd);

            SizeType remain = capacity;
            NodeType* data = GetData();
            while (remain > startIndex)
            {
                SizeType freeIndex = --remain;
                if (FirstFreeNodeIndex != INDEX_NONE)
                {
                    data[FirstFreeNodeIndex].PrevIndex = freeIndex;
                }
                data[freeIndex].PrevIndex = INDEX_NONE;
                data[freeIndex].NextIndex = FirstFreeNodeIndex != INDEX_NONE ? FirstFreeNodeIndex : INDEX_NONE;
                FirstFreeNodeIndex = freeIndex;
                AllocateFlags.Add(false);
            }
            FreeElemCount += elemToAdd;
        }

        bool HasValue(SizeType index) const
        {
            ENSURE(index >= 0 && index < MaxIndex());
            return AllocateFlags[index];
        }

        bool Empty() const
        {
            return Size() == 0;
        }

        SizeType Size() const
        {
            return ElemNodes.Size() - FreeElemCount;
        }

        SizeType MaxIndex() const
        {
            return ElemNodes.Size();
        }

        SizeType Capacity() const
        {
            return ElemNodes.Capacity();
        }

        constexpr SizeType MaxSize() const
        {
            return ElemNodes.MaxSize();
        }

        const NodeType* GetData() const
        {
            return ElemNodes.Data();
        }

        NodeType* GetData()
        {
            return ElemNodes.Data();
        }

        Iterator begin() { return Iterator(*this, AllocateFlags.CreateValidIterator()); }

        ConstIterator begin() const { return ConstIterator(*this, AllocateFlags.CreateValidIterator()); }

        Iterator end() { return Iterator(*this, AllocateFlags.CreateValidIterator(AllocateFlags.Size())); }

        ConstIterator end() const { return ConstIterator(*this, AllocateFlags.CreateValidIterator(AllocateFlags.Size())); }

    private:
        template <typename... Args>
        SizeType Emplace(Args&&... args)
        {
            SizeType index = AddUnconstructElement();
            new(GetData() + index) ValueType(std::forward<Args>(args)...);
            return index;
        }

        void CopyElement(const ValueType* data, SizeType size)
        {
            ENSURE(size >= 0);
            Reserve(size);
            for (SizeType idx = 0; idx < size; ++idx)
            {
                Add(data[idx]);
            }
        }

        SizeType AddUnconstructElement()
        {
            SizeType index = INDEX_NONE;
            if (FirstFreeNodeIndex != INDEX_NONE)
            {
                // return allocated element node
                ENSURE(AllocateFlags[FirstFreeNodeIndex] == false);
                NodeType& node = ElemNodes[FirstFreeNodeIndex];
                index = FirstFreeNodeIndex;
                AllocateFlags[index] = true;
                FirstFreeNodeIndex = node.NextIndex;
                FreeElemCount--;
            }
            else
            {
                // Add new element
                index = ElemNodes.AddUnconstructElement(1);
                AllocateFlags.Add(true);
            }

            return index;
        }

        void InsertUnconstructElement(SizeType index)
        {
            if (index >= ElemNodes.Size())
            {
                Reserve(index + 1);
            }

            ENSURE(AllocateFlags[index] == false);

            FreeElemCount--;
            NodeType& node = ElemNodes[index];
            SizeType prevIndex = node.PrevIndex;
            SizeType nextIndex = node.NextIndex;
            if (prevIndex != INDEX_NONE)
            {
                ElemNodes[prevIndex].NextIndex = nextIndex;
            }

            if (nextIndex != INDEX_NONE)
            {
                ElemNodes[nextIndex].PrevIndex = prevIndex;
            }

            if (FirstFreeNodeIndex == index)
            {
                FirstFreeNodeIndex = nextIndex;
            }

            AllocateFlags[index] = true;
        }

        void RemoveWithoutDestruct(SizeType index, NodeType* node = nullptr)
        {
            if (node == nullptr)
            {
                node = &ElemNodes[index];
            }

            ENSURE(node != nullptr);

            if (FirstFreeNodeIndex != INDEX_NONE)
            {
                ElemNodes[FirstFreeNodeIndex].PrevIndex = index;
            }
            node->NextIndex = FirstFreeNodeIndex;
            node->PrevIndex = INDEX_NONE;
            FirstFreeNodeIndex = index;
            AllocateFlags[index] = false;
            FreeElemCount++;
        }

    private:
        SizeType FirstFreeNodeIndex{ (SizeType)INDEX_NONE };
        SizeType FreeElemCount{0 };
        BitArrayType AllocateFlags;
        ArrayType ElemNodes;
    };





























    template <typename ElementType, typename Allocator = DefaultAllocator>
    class SparseArray_Deprecated
    {
        union ElementLinkNode
        {
            ElementType Element;
            struct
            {
                int32 PrevIndex;
                int32 NextIndex;
            };
        };
    public:
        using TDynamicArray = DynamicArray<ElementLinkNode, Allocator>;
        using TBitArray = BitArrayDeprecated<DefaultAllocator>;
        using BitArrayType = BitArrayDeprecated<DefaultAllocator>;
        using ValueType = ElementType;
        using SizeType = int32;

        template <typename T, typename U, typename V> friend class Set;

    public:
        using ConstIterator = ConstSparseIterator<SparseArray_Deprecated>;
        using Iterator = SparseIterator<SparseArray_Deprecated>;

    public:
        SparseArray_Deprecated() = default;

        explicit SparseArray_Deprecated(int32 capacity)
            : AllocateFlags(TBitArray(capacity))
            , ElementNodes(TDynamicArray(capacity))
        {}

        SparseArray_Deprecated(ElementType* ptr, int32 count)
        {
            CopyElement(ptr, count);
        }

        SparseArray_Deprecated(std::initializer_list<ElementType> initializer)
        {
            CopyElement(initializer.begin(), static_cast<int32>(initializer.size()));
        }

        SparseArray_Deprecated(const SparseArray_Deprecated& other)
            : FirstFreeNodeIndex(other.FirstFreeNodeIndex)
            , FreeElementCount(other.FreeElementCount)
            , AllocateFlags(other.AllocateFlags)
            , ElementNodes(other.ElementNodes)
        {}

        SparseArray_Deprecated(SparseArray_Deprecated&& other) noexcept
            : FirstFreeNodeIndex(other.FirstFreeNodeIndex)
            , FreeElementCount(other.FreeElementCount)
            , AllocateFlags(MoveTemp(other.AllocateFlags))
            , ElementNodes(MoveTemp(other.ElementNodes))
        {
            other.FirstFreeNodeIndex = INDEX_NONE;
            other.FreeElementCount = 0;
        }

        template <typename OtherAllocator>
        explicit SparseArray_Deprecated(const SparseArray_Deprecated<ElementType, OtherAllocator>& other)
            : FirstFreeNodeIndex(other.FirstFreeNodeIndex)
            , FreeElementCount(other.FreeElementCount)
            , AllocateFlags(other.AllocateFlags)
            , ElementNodes(other.ElementNodes)
        {}

        SparseArray_Deprecated& operator= (std::initializer_list<ElementType> initializer)
        {
            Clear(initializer.size());
            CopyElement(initializer.begin(), static_cast<int32>(initializer.size()));
        }

        SparseArray_Deprecated& operator= (const SparseArray_Deprecated& other)
        {
            ENSURE(this != &other);
            FirstFreeNodeIndex = other.FirstFreeNodeIndex;
            FreeElementCount = other.FreeElementCount;
            AllocateFlags = other.AllocateFlags;
            ElementNodes = other.ElementNodes;
            return *this;
        }

        SparseArray_Deprecated& operator= (SparseArray_Deprecated&& other) noexcept
        {
            ENSURE(this != &other);
            FirstFreeNodeIndex = other.FirstFreeNodeIndex;
            other.FirstFreeNodeIndex = INDEX_NONE;
            FreeElementCount = other.FreeElementCount;
            other.FreeElementCount = 0;
            AllocateFlags = MoveTemp(other.AllocateFlags);
            ElementNodes = MoveTemp(other.ElementNodes);
            return *this;
        }

        template <typename OtherAllocator>
        SparseArray_Deprecated& operator= (const SparseArray_Deprecated<ElementType, OtherAllocator>& other)
        {
            FirstFreeNodeIndex = other.FirstFreeNodeIndex;
            FreeElementCount = other.FreeElementCount;
            AllocateFlags = other.AllocateFlags;
            ElementNodes = other.ElementNodes;
            return *this;
        }

        bool operator== (const SparseArray_Deprecated& other) const
        {
            if (GetMaxIndex() != other.GetMaxIndex())
            {
                return false;
            }

            for (int32 idx = 0; idx < GetMaxIndex(); ++idx)
            {
                if (HasElement(idx) != other.HasElement(idx))
                {
                    return false;
                }

                if (HasElement(idx) && (*this)[idx] != other[idx])
                {
                    return false;
                }
            }
            return true;
        }

        ElementType& operator[] (int32 index)
        {
            ENSURE(0 <=index && AllocateFlags[index]);
            return ElementNodes[index].Element;
        }

        const ElementType& operator[] (int32 index) const
        {
            ENSURE(0 <= index && AllocateFlags[index]);
            return ElementNodes[index].Element;
        }

        uint32 Add(const ElementType& element)
        {
            return Emplace(element);
        }

        uint32 Add(ElementType&& element)
        {
            return Emplace(element);
        }

        void Insert(int32 index, const ElementType& element)
        {
            InsertUnconstructElement(index);
            new(GetData() + index) ElementType(element);
        }

        void Insert(int32 index, ElementType&& element)
        {
            InsertUnconstructElement(index);
            new(GetData() + index) ElementType(element);
        }

        void RemoveAt(int32 index)
        {
            ENSURE(0 <= index && index < GetMaxIndex());
            ElementLinkNode& node = ElementNodes[index];
            node.Element.~ElementType();
            RemoveWithoutDestruct(index, &node);
        }

        void Clear(int32 slack)
        {
            if constexpr (HasUserDestructorV<ElementType>)
            {
                typedef ElementType DestructItemsElementTypeTypedef;
                for (Iterator iter = begin(); iter != end(); ++iter)
                {
                    *iter.~DestructItemsElementTypeTypedef();
                }
            }

            FirstFreeNodeIndex = INDEX_NONE;
            FreeElementCount = 0;
            ElementNodes.Clear(slack);
            AllocateFlags.Clear(slack);
        }

        void Reserve(int32 count)
        {
            if (GetMaxIndex() < count)
            {
                int32 elementToAdd = count - GetMaxIndex();
                int32 startIndex = ElementNodes.AddUnconstructElement(elementToAdd);

                int32 remain = count;
                while (remain > startIndex)
                {
                    int32 freeIndex = --remain;
                    if (FirstFreeNodeIndex != INDEX_NONE)
                    {
                        GetData()[FirstFreeNodeIndex].PrevIndex = freeIndex;
                    }
                    GetData()[freeIndex].PrevIndex = INDEX_NONE;
                    GetData()[freeIndex].NextIndex = FirstFreeNodeIndex != INDEX_NONE ? FirstFreeNodeIndex : INDEX_NONE;
                    FirstFreeNodeIndex = freeIndex;
                    //TODO: Add multi value to bit array
                    AllocateFlags.Add(false);
                }
                FreeElementCount += elementToAdd;
            }
        }

        bool HasElement(int32 index) const
        {
            ENSURE(index >= 0 && index < GetMaxIndex());
            return AllocateFlags[index];
        }

        bool Empty() const
        {
            return Size() == 0;
        }

        int32 Size() const
        {
            return ElementNodes.Size() - FreeElementCount;
        }

        int32 GetMaxIndex() const
        {
            return ElementNodes.Size();
        }

        int32 Capacity() const
        {
            return ElementNodes.Capacity();
        }

        const ElementLinkNode* GetData() const
        {
            return ElementNodes.Data();
        }

        ElementLinkNode* GetData()
        {
            return ElementNodes.Data();
        }

        Iterator begin() { return Iterator(*this, AllocateFlags.CreateValidIterator()); }

        ConstIterator begin() const { return ConstIterator(*this, AllocateFlags.CreateValidIterator()); }

        Iterator end() { return Iterator(*this, AllocateFlags.CreateValidIterator(AllocateFlags.Size())); }

        ConstIterator end() const { return ConstIterator(*this, AllocateFlags.CreateValidIterator(AllocateFlags.Size())); }

    private:
        template <typename... Args>
        int32 Emplace(Args&&... args)
        {
            int32 index = AddUnconstructElement();
            new(GetData() + index) ElementType(Forward<Args>(args)...);
            return index;
        }

        void CopyElement(const ElementType* data, int32 count)
        {
            ENSURE(count >= 0);
            Reserve(count);
            for (int32 idx = 0; idx < count; ++idx)
            {
                Add(data[idx]);
            }
        }

        int32 AddUnconstructElement()
        {
            int32 index = INDEX_NONE;
            if (FirstFreeNodeIndex != INDEX_NONE)
            {
                // return allocated element node
                ENSURE(!AllocateFlags[FirstFreeNodeIndex]);
                ElementLinkNode& node = ElementNodes[FirstFreeNodeIndex];
                index = FirstFreeNodeIndex;
                AllocateFlags[index] = true;
                FirstFreeNodeIndex = node.NextIndex;
                FreeElementCount--;
            }
            else
            {
                // add new element
                index = ElementNodes.AddUnconstructElement(1);
                AllocateFlags.Add(true);
            }

            return index;
        }

        void InsertUnconstructElement(int32 index)
        {
            if (index >= ElementNodes.Size())
            {
                Reserve(index + 1);
            }

            ENSURE(AllocateFlags[index] == false);

            FreeElementCount--;
            ElementLinkNode& node = ElementNodes[index];
            int32 prevIndex = node.PrevIndex;
            int32 nextIndex = node.NextIndex;
            if (prevIndex != INDEX_NONE)
            {
                ElementNodes[prevIndex].NextIndex = nextIndex;
            }

            if (nextIndex != INDEX_NONE)
            {
                ElementNodes[nextIndex].PrevIndex = prevIndex;
            }

            if (FirstFreeNodeIndex == index)
            {
                FirstFreeNodeIndex = nextIndex;
            }

            AllocateFlags[index] = true;
        }

        void RemoveWithoutDestruct(int32 index, ElementLinkNode* node = nullptr)
        {
            if (node == nullptr)
            {
                node = &ElementNodes[index];
            }

            ENSURE(node != nullptr);

            if (FirstFreeNodeIndex != INDEX_NONE)
            {
                ElementNodes[FirstFreeNodeIndex].PrevIndex = index;
            }
            node->NextIndex = FirstFreeNodeIndex;
            node->PrevIndex = INDEX_NONE;
            FirstFreeNodeIndex = index;
            AllocateFlags[index] = false;
            FreeElementCount++;
        }
    private:
        int32 FirstFreeNodeIndex{ INDEX_NONE };
        int32 FreeElementCount{ 0 };
        TBitArray AllocateFlags;
        TDynamicArray ElementNodes;
    };
}