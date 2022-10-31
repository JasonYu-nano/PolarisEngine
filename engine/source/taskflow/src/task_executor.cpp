#include "task_executor.hpp"

namespace Engine
{
    TaskExecutor GExecutor(std::thread::hardware_concurrency());

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

    TaskExecutor::TaskExecutor(uint32 threadNum)
    {
        Worker = MakeUnique<ThreadPool>(threadNum);
        Worker->Setup();
    }
}
