#pragma once

#include "graph_node.hpp"

namespace Engine
{
    class TASKFLOW_API Graph
    {
    public:
        using NodeIterator = DynamicArray<GraphNode*>::Iterator;
        using ConstNodeIterator = DynamicArray<GraphNode*>::ConstIterator;

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

        NodeIterator begin() { return Nodes.begin(); }

        ConstNodeIterator begin() const { return Nodes.begin(); }

        NodeIterator end() { return Nodes.end(); }

        ConstNodeIterator end() const { return Nodes.end(); }

    private:
        DynamicArray<GraphNode*> Nodes;
    };
}