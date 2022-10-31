#pragma once

#include "taskflow.hpp"
#include "thread/thread_pool.hpp"
#include "foundation/smart_ptr.hpp"

namespace Engine
{
    class TASKFLOW_API TaskExecutor
    {
    public:
        explicit TaskExecutor(uint32 threadNum);

        void Execute(const Taskflow& tf);

    private:
        UniquePtr<ThreadPool> Worker;
        //TODO: Change to queue
        DynamicArray<GraphNode*> IndependentTasks;
    };

    extern TASKFLOW_API TaskExecutor GExecutor;
}