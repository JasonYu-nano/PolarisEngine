#pragma once

#include "foundation/bit_array.hpp"
#include "foundation/dynamic_array.hpp"
#include "foundation/type_traits.hpp"
#include "predefine/common_marco.hpp"

namespace Engine
{
#pragma region iterator
    template <typename ContainerType, typename ElementType, typename BitIteratorType>
    class ConstSparseIterator
    {
    public:
        ConstSparseIterator(const ContainerType& container, BitIteratorType bitIter)
            : Container(container)
            , BitIterator(bitIter)
        {}

        const ElementType& operator*() const { return Container[GetIndex()]; }

        const ElementType* operator->() const { return &Container[GetIndex()]; }

        ConstSparseIterator& operator++ ()
        {
            ++BitIterator;
            return *this;
        }

        int32 GetIndex() const { return BitIterator.GetIndex(); }

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
        BitIteratorType BitIterator;
    };

    template <typename ContainerType, typename ElementType, typename BitIteratorType>
    class SparseIterator : public ConstSparseIterator<ContainerType, ElementType, BitIteratorType>
    {
        using Super = ConstSparseIterator<ContainerType, ElementType, BitIteratorType>;
    public:
        SparseIterator(ContainerType& container, BitIteratorType bitIter)
            : Super(container, bitIter)
        {}

        ElementType& operator*() const { return const_cast<ElementType&>(Super::operator*()); }

        ElementType* operator->() const { return const_cast<ElementType*>(Super::operator->()); }

        SparseIterator& operator++ ()
        {
            Super::operator++();
            return *this;
        }

        int32 GetIndex() const { return Super::GetIndex(); }

        friend bool operator== (const SparseIterator& lhs, const SparseIterator& rhs)
        {
            return (&lhs.Container == &rhs.Container) && (lhs.BitIterator == rhs.BitIterator);
        }

        friend bool operator!= (const SparseIterator& lhs, const SparseIterator& rhs)
        {
            return !(lhs == rhs);
        }
    };
#pragma endregion iterator

    template <typename ElementType, typename Allocator = HeapAllocator<int32>>
    class SparseArray
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

        using TDynamicArray = DynamicArray<ElementLinkNode, Allocator>;
        using TBitArray = BitArray<HeapAllocator<uint32>>;

        template <typename T, typename U, typename V> friend class Set;

    public:
        using ConstIterator = ConstSparseIterator<SparseArray, ElementType, TBitArray::ConstValidIterator>;
        using Iterator = SparseIterator<SparseArray, ElementType, TBitArray::ConstValidIterator>;

    public:
        SparseArray() = default;

        explicit SparseArray(int32 capacity)
            : AllocateFlags(TBitArray(capacity))
            , ElementNodes(TDynamicArray(capacity))
        {}

        SparseArray(ElementType* ptr, int32 count)
        {
            CopyElement(ptr, count);
        }

        SparseArray(std::initializer_list<ElementType> initializer)
        {
            CopyElement(initializer.begin(), static_cast<int32>(initializer.size()));
        }

        SparseArray(const SparseArray& other)
            : FirstFreeNodeIndex(other.FirstFreeNodeIndex)
            , FreeElementCount(other.FreeElementCount)
            , AllocateFlags(other.AllocateFlags)
            , ElementNodes(other.ElementNodes)
        {}

        SparseArray(SparseArray&& other) noexcept
            : FirstFreeNodeIndex(other.FirstFreeNodeIndex)
            , FreeElementCount(other.FreeElementCount)
            , AllocateFlags(MoveTemp(other.AllocateFlags))
            , ElementNodes(MoveTemp(other.ElementNodes))
        {
            other.FirstFreeNodeIndex = INDEX_NONE;
            other.FreeElementCount = 0;
        }

        template <typename OtherAllocator>
        explicit SparseArray(const SparseArray<ElementType, OtherAllocator>& other)
            : FirstFreeNodeIndex(other.FirstFreeNodeIndex)
            , FreeElementCount(other.FreeElementCount)
            , AllocateFlags(other.AllocateFlags)
            , ElementNodes(other.ElementNodes)
        {}

        SparseArray& operator= (std::initializer_list<ElementType> initializer)
        {
            Clear(initializer.size());
            CopyElement(initializer.begin(), static_cast<int32>(initializer.size()));
        }

        SparseArray& operator= (const SparseArray& other)
        {
            PL_ASSERT(this != &other);
            FirstFreeNodeIndex = other.FirstFreeNodeIndex;
            FreeElementCount = other.FreeElementCount;
            AllocateFlags = other.AllocateFlags;
            ElementNodes = other.ElementNodes;
            return *this;
        }

        SparseArray& operator= (SparseArray&& other) noexcept
        {
            PL_ASSERT(this != &other);
            FirstFreeNodeIndex = other.FirstFreeNodeIndex;
            other.FirstFreeNodeIndex = INDEX_NONE;
            FreeElementCount = other.FreeElementCount;
            other.FreeElementCount = 0;
            AllocateFlags = MoveTemp(other.AllocateFlags);
            ElementNodes = MoveTemp(other.ElementNodes);
            return *this;
        }

        template <typename OtherAllocator>
        SparseArray& operator= (const SparseArray<ElementType, OtherAllocator>& other)
        {
            FirstFreeNodeIndex = other.FirstFreeNodeIndex;
            FreeElementCount = other.FreeElementCount;
            AllocateFlags = other.AllocateFlags;
            ElementNodes = other.ElementNodes;
            return *this;
        }

        bool operator== (const SparseArray& other) const
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
            PL_ASSERT(0 <=index && AllocateFlags[index]);
            return ElementNodes[index].Element;
        }

        const ElementType& operator[] (int32 index) const
        {
            PL_ASSERT(0 <= index && AllocateFlags[index]);
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
            PL_ASSERT(0 <= index && index < GetMaxIndex());
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
            PL_ASSERT(index >= 0 && index < GetMaxIndex());
            return AllocateFlags[index];
        }

        bool IsEmpty() const
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
            PL_ASSERT(count >= 0);
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
                PL_ASSERT(!AllocateFlags[FirstFreeNodeIndex]);
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

            PL_ASSERT(AllocateFlags[index] == false);

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

            PL_ASSERT(node != nullptr);

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