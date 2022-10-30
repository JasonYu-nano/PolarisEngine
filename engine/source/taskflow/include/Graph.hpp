#pragma once

#include "GraphNode.hpp"

namespace Engine
{
    class TASKFLOW_API Graph
    {
    public:
        void Add(GraphNode* newNode)
        {
            Nodes.Add(newNode);
        }

        void Clear()
        {
            for (GraphNode* node : Nodes)
            {
                delete node;
            }
            Nodes.Clear();
        }

        bool Empty() const
        {
            return Nodes.IsEmpty();
        }

        int32 Size() const
        {
            return Nodes.Size();
        }

    private:
        DynamicArray<GraphNode*> Nodes;
    };
}