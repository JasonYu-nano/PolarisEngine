#include <iostream>
#include "thread/thread_pool.hpp"

namespace Engine
{
    void WorkThread::operator()()
    {

    }

    WorkThread::WorkThread(ThreadPool& owner)
        : Owner(owner)
    {
        Thread = std::jthread([this]{
            std::mutex mutex;
            std::unique_lock<std::mutex> lock(mutex);
            while (!Stop)
            {
                Condition.wait(lock, [this] { return Task != nullptr; });

                IWorkThreadTask* localTask = Task;
                Task = nullptr;

                while (localTask)
                {
                    localTask->Run();
                    localTask = Owner.GetNextTask(*this);
                }
            }
        });
    }

    WorkThread::WorkThread(WorkThread&& other) noexcept
        : Owner(other.Owner)
        , Task(other.Task)
        , Thread(MoveTemp(other.Thread))
    {}

    void ThreadPool::Create(int32 threadNum)
    {
        if (threadNum <= 0)
        {
            return;
        }

        AllWorkers.Reserve(threadNum);

        for (int32 idx = 0; idx < threadNum; ++idx)
        {
            //TODO: Name thread
            WorkThread* thread = new WorkThread(*this);
            AllWorkers.Add(thread);
            IdleWorkers.push(thread);
        }
    }

    void ThreadPool::Destroy()
    {
        for (auto&& worker : AllWorkers)
        {
            worker->Stop = true;
        }
        AllWorkers.Clear();
    }

    void ThreadPool::AddTask(IWorkThreadTask* task)
    {
        ENSURE(task);

        WorkThread* worker = nullptr;
        {
            std::scoped_lock lock(WorkerQueueMutex);
            if (!IdleWorkers.empty())
            {
                worker = IdleWorkers.front();
                IdleWorkers.pop();
            }
        }

        if (worker)
        {
            worker->Task = task;
            worker->Condition.notify_one();
        }
        else
        {
            std::scoped_lock lock(TaskQueueMutex);
            TaskQueue.push(task);
        }
    }

    IWorkThreadTask* ThreadPool::GetNextTask(WorkThread& worker)
    {
        {
            std::scoped_lock lock(TaskQueueMutex);
            if (!TaskQueue.empty())
            {
                IWorkThreadTask* task = TaskQueue.front();
                TaskQueue.pop();
                return task;
            }
        }

        {
            std::scoped_lock lock(WorkerQueueMutex);
            IdleWorkers.push(&worker);
        }

        return nullptr;
    }
}