#include "task_executor.hpp"

namespace Engine
{

    void TaskExecutor::Execute(const Taskflow& tf)
    {
        const Graph& graph = tf.GetGraph();
        for (const GraphNode* node : graph)
        {
            if (node->DependencyNum() <= 0)
            {
                IndependentTasks.Add(const_cast<GraphNode*>(node));
            }
        }
    }
}
