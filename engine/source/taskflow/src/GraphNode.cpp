#include "GraphNode.hpp"

namespace Engine
{

    void GraphNode::Precede(GraphNode* node)
    {
        if (node != this)
        {
            Successors.AddUnique(node);
            node->Dependencies.Add(this);
        }
    }
}