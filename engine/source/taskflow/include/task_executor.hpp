#pragma once

#include "taskflow.hpp"

namespace Engine
{
    class TASKFLOW_API TaskExecutor
    {
    public:
        explicit TaskExecutor(uint32 numThread);

        void Execute(const Taskflow& tf);

    private:
        //TODO: Change to queue
        DynamicArray<GraphNode*> IndependentTasks;
    };
}