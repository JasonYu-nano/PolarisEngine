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
        for (const GraphTaskBase* task : tf)
        {
            if (task->DependencyNum() <= 0)
            {
                task->ConditionDispatch();
            }
        }
    }
}
