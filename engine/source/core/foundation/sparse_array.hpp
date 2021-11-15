#pragma once

#include "foundation/bit_array.hpp"
#include "foundation/list.hpp"

namespace Engine
{
    template <typename ElementType>
    class SparseArray
    {
        #define INVALID_LINK_NODE_INDEX MAX_UINT32

        union ElementLinkNode
        {
            ElementType Element;
            struct
            {
                uint32 PrevIndex;
                uint32 NextIndex;
            };
        };

    public:
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
            if (FirstFreeNodeIndex != INVALID_LINK_NODE_INDEX)
            {
                ElementNodes[FirstFreeNodeIndex].PrevIndex = index;
            }
            node.NextIndex = FirstFreeNodeIndex;
            node.PrevIndex = INVALID_LINK_NODE_INDEX;
            FirstFreeNodeIndex = index;
            AllocateFlags[index] = false;
        }

        uint32 GetCount() const
        {
            return ElementNodes.GetCount();
        }

        ElementLinkNode* GetData()
        {
            return ElementNodes.GetData();
        }

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
            if (GetCount() >= INVALID_LINK_NODE_INDEX)
            {
                throw std::overflow_error();
            }

            uint32 index = INVALID_LINK_NODE_INDEX;
            if (FirstFreeNodeIndex != INVALID_LINK_NODE_INDEX)
            {
                // return allocated element node
                PL_ASSERT(!AllocateFlags[FirstFreeNodeIndex]);
                ElementLinkNode& node = ElementNodes[FirstFreeNodeIndex];
                FirstFreeNodeIndex = node.NextIndex;
                index = FirstFreeNodeIndex;
            }
            else
            {
                // add new element
                index = ElementNodes.AddUnconstructElement(1);
            }
            AllocateFlags[index] = true;

            return index;
        }
    private:
        int32 FirstFreeNodeIndex{ INVALID_LINK_NODE_INDEX };
        BitArray<HeapAllocator<uint32>> AllocateFlags;
        List<ElementLinkNode> ElementNodes;
    };
}