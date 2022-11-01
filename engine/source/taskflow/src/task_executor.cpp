#include "task_executor.hpp"

namespace Engine
{
    TaskExecutor GExecutor = TaskExecutor();

    UniquePtr<ThreadPool> GTaskflowThreadPool = MakeUnique<ThreadPool>();

    ThreadPoolInitializer::ThreadPoolInitializer()
    {
        GTaskflowThreadPool->Create(static_cast<int32>(std::thread::hardware_concurrency()));
    }

    ThreadPoolInitializer TaskExecutor::Initializer = ThreadPoolInitializer();

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
