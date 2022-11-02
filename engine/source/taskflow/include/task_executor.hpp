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

    class TASKFLOW_API TaskExecutor
    {
    public:
        TaskExecutor() = default;

        void Execute(const Taskflow& tf);

    private:
        static ThreadPoolInitializer Initializer;
    };

    extern TASKFLOW_API TaskExecutor GExecutor;

    extern UniquePtr<ThreadPool> GTaskflowThreadPool;
}