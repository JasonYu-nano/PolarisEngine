#include "task_executor.hpp"

namespace Engine
{
    TaskExecutor GExecutor = TaskExecutor();

    UniquePtr<ThreadPool> GTaskflowThreadPool = MakeUnique<ThreadPool>();

    ThreadPoolInitializer::ThreadPoolInitializer()
    {
        //GTaskflowThreadPool->Create(static_cast<int32>(std::thread::hardware_concurrency()));
        GTaskflowThreadPool->Create(1);
    }

    ThreadPoolInitializer TaskExecutor::Initializer = ThreadPoolInitializer();

    void TaskExecutor::Execute(Taskflow& tf)
    {
        for (GraphTaskBase* task : tf)
        {
            if (task->DependencyNum() <= 0)
            {
                task->ConditionDispatch();
            }
        }
    }

    void TaskExecutor::DispatchTask(GraphTaskBase* task)
    {
        ENSURE(task);
        GTaskflowThreadPool->AddTask(task);
    }
}
