#pragma once

#include "foundation/bit_array.hpp"
#include "foundation/array.hpp"
#include "foundation/array.hpp"
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

        explicit operator bool() const
        {
            return (bool)BitIterator;
        }

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
        template <typename T, typename U, typename V> friend class Set;
    public:
        using ValueType = Elem;
        using SizeType = typename Alloc::SizeType;

        union _SparseArrayLinkNode
        {
            ValueType& GetVal()
            {
                return *Val.GetData();
            }

            const ValueType& GetVal() const
            {
                return *Val.GetData();
            }

            UntypedData<ValueType> Val;
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

        explicit SparseArray(const AllocatorType& alloc = AllocatorType())
            : AllocateFlags()
            , ElemNodes(alloc)
        {}

        explicit SparseArray(SizeType capacity, const AllocatorType& alloc = AllocatorType())
            : AllocateFlags(capacity)
            , ElemNodes(capacity, alloc)
        {}

        SparseArray(ValueType* ptr, SizeType size, const AllocatorType& alloc = AllocatorType())
            : AllocateFlags()
            , ElemNodes(alloc)
        {
            CopyAssign(ptr, size);
        }

        SparseArray(std::initializer_list<ValueType> initializer, const AllocatorType& alloc = AllocatorType())
            : AllocateFlags()
            , ElemNodes(alloc)
        {
            CopyAssign(initializer.begin(), static_cast<SizeType>(initializer.size()));
        }

        SparseArray(const SparseArray& other)
        {
            CopyAssign(other);
        }

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
            CopyAssign(initializer.begin(), static_cast<SizeType>(initializer.size()));
        }

        SparseArray& operator= (const SparseArray& other)
        {
            ENSURE(this != &other);
            CopyAssign(other);
            return *this;
        }

        SparseArray& operator= (SparseArray&& other) noexcept
        {
            ENSURE(this != &other);
            MoveAssign(std::forward<SparseArray>(other));
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
            return *ElemNodes[index].Val.GetData();
        }

        const ValueType& operator[] (SizeType index) const
        {
            ENSURE(0 <= index && AllocateFlags[index]);
            return *ElemNodes[index].Val.GetData();
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
            new(Data() + index) ValueType(element);
        }

        void Insert(SizeType index, ValueType&& element)
        {
            InsertUnconstructElement(index);
            new(Data() + index) ValueType(element);
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
            NodeType* data = Data();
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

        const NodeType* Data() const
        {
            return ElemNodes.Data();
        }

        NodeType* Data()
        {
            return ElemNodes.Data();
        }

        void Shrink()
        {
            SizeType lastAllocatedIndex = AllocateFlags.FindLast(true);

            const SizeType firstIndexToRemove = lastAllocatedIndex + 1;
            if(firstIndexToRemove < ElemNodes.Size())
            {
                if(FreeElemCount > 0)
                {
                    // Look for elements in the free list that are in the memory to be freed.
                    SizeType freeIndex = FirstFreeNodeIndex;
                    NodeType* data = Data();
                    while(freeIndex != INDEX_NONE)
                    {
                        NodeType& node = data[freeIndex];
                        if(freeIndex < firstIndexToRemove)
                        {
                            freeIndex = node.NextIndex;
                        }
                        else
                        {
                            const SizeType prevFreeIndex = node.PrevIndex;
                            const SizeType nextFreeIndex = node.NextIndex;
                            if(nextFreeIndex != -1)
                            {
                                node.PrevIndex = prevFreeIndex;
                            }
                            if(prevFreeIndex != -1)
                            {
                                node.NextIndex = nextFreeIndex;
                            }
                            else
                            {
                                FirstFreeNodeIndex = nextFreeIndex;
                            }
                            --FreeElemCount;

                            freeIndex = nextFreeIndex;
                        }
                    }
                }

                // Truncate unallocated elements at the end of the data array.
                ElemNodes.RemoveAt(firstIndexToRemove, ElemNodes.Size() - firstIndexToRemove);
                AllocateFlags.RemoveAt(firstIndexToRemove, AllocateFlags.Size() - firstIndexToRemove);
            }

            // Shrink the data array.
            ElemNodes.Shrink();
        }

        bool Compact()
        {
            SizeType freeCount = FreeElemCount;
            if (freeCount == 0)
            {
                return false;
            }

            bool result = false;

            NodeType* nodeData = ElemNodes.Data();

            SizeType endIndex    = ElemNodes.Size();
            SizeType targetIndex = endIndex - freeCount;
            SizeType freeIndex   = FirstFreeNodeIndex;
            while (freeIndex != INDEX_NONE)
            {
                SizeType nextFreeIndex = nodeData[freeIndex].NextIndex;
                if (freeIndex < targetIndex)
                {
                    // We need an element here
                    do
                    {
                        --endIndex;
                    }
                    while (!AllocateFlags[endIndex]);

                    RelocateElements(nodeData + freeIndex, nodeData + endIndex, 1);
                    AllocateFlags[freeIndex] = true;

                    result = true;
                }

                freeIndex = nextFreeIndex;
            }

            ElemNodes.RemoveAt(targetIndex, freeCount);
            AllocateFlags.RemoveAt(targetIndex, freeCount);

            FreeElemCount = 0;
            FirstFreeNodeIndex = INDEX_NONE;

            return result;
        }

        void Sort()
        {
            if (Size() > 0)
            {
                Compact();
                ElemNodes.Sort();
            }
        }

        template <typename Predicate>
        void Sort(Predicate pred)
        {
            if (Size() > 0)
            {
                Compact();
                ElemNodes.Sort(pred);
            }
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
            new(Data() + index) ValueType(std::forward<Args>(args)...);
            return index;
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

        void CopyAssign(const SparseArray& other)
        {
            FirstFreeNodeIndex = other.FirstFreeNodeIndex;
            FreeElemCount = other.FreeElemCount;
            AllocateFlags = other.AllocateFlags;
            ElemNodes.GetAlloc() = other.ElemNodes.GetAlloc();

            if (other.ElemNodes.Empty())
            {
                ElemNodes.Clear();
                return;
            }

            if constexpr (std::is_trivially_copyable_v<ValueType>)
            {
                // Just memory copy
                ElemNodes = other.ElemNodes;
                return;
            }

            SizeType otherSize = other.ElemNodes.Size();
            ElemNodes.Reserve(other.ElemNodes.Capacity());
            ElemNodes.AddUnconstructElement(otherSize);
            NodeType* destData = ElemNodes.Data();
            const NodeType* srcData = other.ElemNodes.Data();

            for (SizeType idx = 0; idx < otherSize; ++idx)
            {
                NodeType& dest = destData[idx];
                const NodeType& src = srcData[idx];

                if (other.HasValue(idx))
                {
                    new(&dest.Val) ValueType(*src.Val.GetData());
                }
                else
                {
                    dest.NextIndex = src.NextIndex;
                    dest.PrevIndex = dest.PrevIndex;
                }
            }
        }

        void CopyAssign(const ValueType* data, SizeType size)
        {
            ENSURE(size >= 0);
            Reserve(size);
            for (SizeType idx = 0; idx < size; ++idx)
            {
                Add(data[idx]);
            }
        }

        void MoveAssign(SparseArray&& other)
        {
            FirstFreeNodeIndex = other.FirstFreeNodeIndex;
            FreeElemCount = other.FreeElemCount;
            AllocateFlags = std::move(other.AllocateFlags);
            ElemNodes = std::move(other.ElemNodes);

            other.FirstFreeNodeIndex = INDEX_NONE;
            other.FreeElemCount = 0;
        }

    private:
        SizeType FirstFreeNodeIndex{ (SizeType)INDEX_NONE };
        SizeType FreeElemCount{ 0 };
        BitArrayType AllocateFlags;
        ArrayType ElemNodes;
    };
}