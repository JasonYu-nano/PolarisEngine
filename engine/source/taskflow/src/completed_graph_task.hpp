#pragma once

#include "graph_task.hpp"
#include "foundation/delegate.hpp"

namespace Engine
{
    DECLARE_DELEGATE(OnTaskCompletedDelegate);

    class CompletedGraphTask : public GraphTaskBase
    {
    public:
        void Precede(GraphTaskBase* node) override
        {
            // CompletedGraphTask must be the last task
            ENSURE(0);
        }

        void Run() override
        {
            OnTaskCompleted.ExecuteIfBound();
        }

        OnTaskCompletedDelegate OnTaskCompleted;
    };
}
