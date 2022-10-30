#pragma once

#include "definitions_taskflow.hpp"
#include "foundation/dynamic_array.hpp"

namespace Engine
{
    class TASKFLOW_API GraphNode
    {
    public:

        void Precede(GraphNode* node);

        GraphNode& operator-- ()
        {
            return *this;
        }

        GraphNode& operator> (GraphNode* node)
        {
            node->Precede(this);
            return *this;
        }

        GraphNode& operator< (GraphNode* node)
        {
            Precede(node);
            return *this;
        }

    private:
        DynamicArray<GraphNode*> Successors;
        DynamicArray<GraphNode*> Dependencies;
    };
}