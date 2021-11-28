#pragma once

#include "foundation/bit_array.hpp"
#include "foundation/dynamic_array.hpp"
#include "foundation/type_traits.hpp"

namespace Engine
{
    constexpr uint32 kSparseArrayIndexNone = MAX_UINT32;

    template <typename ContainerType, typename ElementType, typename BitIterator>
    class ConstSparseIterator
    {
    public:
        ConstSparseIterator(const ContainerType& container, BitIterator bitIter)
            : Container(container)
            , BitIterator(bitIter)
        {}

        const ElementType& operator*() const { return Container[GetIndex()]; }

        const ElementType& operator->() const { return Container[GetIndex()]; }

        ConstSparseIterator& operator++ ()
        {
            ++BitIterator;
            return *this;
        }

        uint32 GetIndex() const { return BitIterator.GetIndex(); }

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
        BitIterator BitIterator;
    };

    template <typename ContainerType, typename ElementType, typename BitIterator>
    class SparseIterator : public ConstSparseIterator<ContainerType, ElementType, BitIterator>
    {
        using Super = ConstSparseIterator<ContainerType, ElementType, BitIterator>;
    public:
        SparseIterator(ContainerType& container, BitIterator bitIter)
            : Super(container, bitIter)
        {}

        ElementType& operator*() const { return const_cast<ElementType&>(Super::operator*()); }

        ElementType& operator->() const { return const_cast<ElementType&>(Super::operator->()); }

        SparseIterator& operator++ ()
        {
            Super::operator++();
            return *this;
        }

        uint32 GetIndex() const { return Super::GetIndex(); }

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
                uint32 PrevIndex;
                uint32 NextIndex;
            };
        };

        typedef BitArray<HeapAllocator<uint32>> TBitArray;

        template <typename T, typename U, typename V> friend class Set;

    public:
        using ConstIterator = ConstSparseIterator<SparseArray, ElementType, TBitArray::Iterator>;
        using Iterator = SparseIterator<SparseArray, ElementType, TBitArray::Iterator>;

    public:
        SparseArray() = default;

        SparseArray(uint32 capacity)
            : AllocateFlags(BitArray(capacity))
            , ElementNodes(DynamicArray<ElementLinkNode>(capacity))
        {}

        ElementType& operator[] (uint32 index)
        {
            PL_ASSERT(AllocateFlags[index]);
            return ElementNodes[index].Element;
        }

        const ElementType& operator[] (uint32 index) const
        {
            PL_ASSERT(AllocateFlags[index]);
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

        void RemoveAt(uint32 index)
        {
            PL_ASSERT(index < GetCount());
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

        uint32 GetCount() const
        {
            return ElementNodes.GetCount();
        }

        uint32 GetCapacity() const
        {
            return ElementNodes.GetCapacity();
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
        uint32 Emplace(Args&&... args)
        {
            uint32 index = AddUnconstructElement();
            new(GetData() + index) ElementType(Forward<Args>(args)...);
            return index;
        }

        uint32 AddUnconstructElement()
        {
            if (GetCount() >= kSparseArrayIndexNone)
            {
                throw std::overflow_error("index of sparse array can not >= MAX_UINT32");
            }

            uint32 index = kSparseArrayIndexNone;
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
        uint32 FirstFreeNodeIndex{ kSparseArrayIndexNone };
        TBitArray AllocateFlags;
        DynamicArray<ElementLinkNode> ElementNodes;
    };
}