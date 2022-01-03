#pragma once

#include "foundation/bit_array.hpp"
#include "foundation/dynamic_array.hpp"
#include "foundation/type_traits.hpp"

namespace Engine
{
    constexpr int32 kSparseArrayIndexNone = -1;

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

    template <typename ElementType>
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

        using TDynamicArray = DynamicArray<ElementLinkNode, HeapAllocator<int32>>;
        using TBitArray = BitArray<HeapAllocator<uint32>>;

        template <typename T, typename U, typename V> friend class Set;

    public:
        using ConstIterator = ConstSparseIterator<SparseArray, ElementType, TBitArray::Iterator>;
        using Iterator = SparseIterator<SparseArray, ElementType, TBitArray::Iterator>;

    public:
        SparseArray() = default;

        explicit SparseArray(int32 capacity)
            : AllocateFlags(TBitArray(capacity))
            , ElementNodes(TDynamicArray(capacity))
        {}

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

        void RemoveAt(int32 index)
        {
            PL_ASSERT(0 <= index && index < GetCount());
            ElementLinkNode& node = ElementNodes[index];
            if (FirstFreeNodeIndex != kSparseArrayIndexNone)
            {
                ElementNodes[FirstFreeNodeIndex].PrevIndex = index;
            }
            node.NextIndex = FirstFreeNodeIndex;
            node.PrevIndex = kSparseArrayIndexNone;
            FirstFreeNodeIndex = index;
            AllocateFlags[index] = false;
        }

        void Reserve(int32 count)
        {
            if (GetCount() < count)
            {
                int32 elementToAdd = count - GetCount();
                int32 startIndex = ElementNodes.AddUnconstructElement(elementToAdd);

                int32 remain = count;
                while (remain > startIndex)
                {
                    int32 freeIndex = --remain;
                    if (FirstFreeNodeIndex != kSparseArrayIndexNone)
                    {
                        GetData()[FirstFreeNodeIndex].PrevIndex = freeIndex;
                    }
                    GetData()[freeIndex].PrevIndex = kSparseArrayIndexNone;
                    GetData()[freeIndex].NextIndex = FirstFreeNodeIndex != kSparseArrayIndexNone ? FirstFreeNodeIndex : kSparseArrayIndexNone;
                    FirstFreeNodeIndex = freeIndex;
                    //TODO: Add multi value to bit array
                    AllocateFlags.Add(false);
                }
            }
        }

        int32 GetCount() const
        {
            return ElementNodes.GetCount();
        }

        int32 GetCapacity() const
        {
            return ElementNodes.GetCapacity();
        }

        const ElementLinkNode* GetData() const
        {
            return ElementNodes.GetData();
        }

        ElementLinkNode* GetData()
        {
            return ElementNodes.GetData();
        }

        Iterator begin() { return Iterator(*this, TBitArray::Iterator(AllocateFlags, 0)); }

        ConstIterator begin() const { return ConstIterator(*this, TBitArray::Iterator(AllocateFlags, 0)); }

        Iterator end() { return Iterator(*this, TBitArray::Iterator(AllocateFlags, GetCount())); }

        ConstIterator end() const { return ConstIterator(*this, TBitArray::Iterator(AllocateFlags, GetCount())); }

    private:
        template <typename... Args>
        int32 Emplace(Args&&... args)
        {
            int32 index = AddUnconstructElement();
            new(GetData() + index) ElementType(Forward<Args>(args)...);
            return index;
        }

        int32 AddUnconstructElement()
        {
            int32 index = kSparseArrayIndexNone;
            if (FirstFreeNodeIndex != kSparseArrayIndexNone)
            {
                // return allocated element node
                PL_ASSERT(!AllocateFlags[FirstFreeNodeIndex]);
                ElementLinkNode& node = ElementNodes[FirstFreeNodeIndex];
                index = FirstFreeNodeIndex;
                AllocateFlags[index] = true;
                FirstFreeNodeIndex = node.NextIndex;
            }
            else
            {
                // add new element
                index = ElementNodes.AddUnconstructElement(1);
                AllocateFlags.Add(true);
            }

            return index;
        }
    private:
        int32 FirstFreeNodeIndex{ kSparseArrayIndexNone };
        TBitArray AllocateFlags;
        TDynamicArray ElementNodes;
    };
}