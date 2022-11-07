#pragma once

#include "taskflow.hpp"
#include "thread/thread_pool.hpp"
#include "foundation/smart_ptr.hpp"

namespace Engine
{
    struct ThreadPoolInitializer
    {
        ThreadPoolInitializer();
    };

    class TaskExecutor
    {
    public:
        static TaskExecutor& Get()
        {
            static TaskExecutor executor;
            return executor;
        }

        TaskExecutor() = default;

        void Execute(Taskflow& tf);

        void DispatchTask(GraphTaskBase* task);
    private:
        static ThreadPoolInitializer Initializer;
    };

    extern TASKFLOW_API TaskExecutor GExecutor;

    extern UniquePtr<ThreadPool> GTaskflowThreadPool;
}