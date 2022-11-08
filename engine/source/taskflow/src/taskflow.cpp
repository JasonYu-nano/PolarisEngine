#include "taskflow.hpp"
#include "completed_graph_task.hpp"
#include "task_executor.hpp"

namespace Engine
{
    void Taskflow::Execute()
    {
        ENSURE(Executable);
        Executable = false;
        CompletedGraphTask* lastTask = new CompletedGraphTask();
        lastTask->OnTaskCompleted.BindRaw(this, &Taskflow::OnLastTaskCompleted);

        for (auto task : Tasks)
        {
            task->Precede(lastTask);
        }
        Tasks.Add(lastTask);

        TaskExecutor::Get().Execute(*this);
    }

    void Taskflow::Wait()
    {
        if (Executable)
        {
            return;
        }

        if (!PromiseRetrieved)
        {
            PromiseRetrieved = true;
            Promise.get_future().wait();
        }
    }

    void Taskflow::Clear()
    {
        for (GraphTaskBase* node : Tasks)
        {
            delete node;
        }
        Tasks.Clear();
    }
}
