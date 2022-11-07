#include "graph_task.hpp"
#include "task_executor.hpp"

namespace Engine
{
    void GraphTaskBase::Precede(GraphTaskBase* node)
    {
        if (node != this)
        {
            Subsequences.AddUnique(node);
            node->Prerequisites.Add(this);
            ++(node->WaitingPrerequisites);
        }
    }

    void GraphTaskBase::ConditionDispatch()
    {
        if (WaitingPrerequisites > 0)
        {
            --WaitingPrerequisites;
        }

        if (WaitingPrerequisites > 0)
        {
            return;
        }

        TaskExecutor::Get().DispatchTask(const_cast<GraphTaskBase*>(this));
    }
}