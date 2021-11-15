#pragma once

#include "foundation/bit_array.hpp"
#include "foundation/list.hpp"

namespace Engine
{
    template <typename ElementType>
    class SparseArray
    {
        #define INVALID_LINK_NODE_INDEX ((int32) -1)

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
        uint32 Add(const ElementType& element)
        {
            return Emplace(element);
        }

        uint32 Add(ElementType&& element)
        {
            return Emplace(element);
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
            int32 index = INVALID_LINK_NODE_INDEX;
            if (FirstFreeNodeIndex != INVALID_LINK_NODE_INDEX)
            {
                // return allocated element node
                PL_ASSERT(!AllocateFlags[FirstFreeNodeIndex]);
                ElementLinkNode& node = ElementNodes[FirstFreeNodeIndex];
                if (node.PrevIndex != INVALID_LINK_NODE_INDEX)
                {
                    ElementLinkNode& prevNode = ElementNodes[node.PrevIndex];
                    prevNode.NextIndex = node.NextIndex;
                }
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