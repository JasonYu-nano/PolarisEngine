#include "thread/thread_pool.hpp"

namespace Engine
{
    void PooledThread::operator()()
    {

    }

    PooledThread::PooledThread()
    {
        Thread = std::jthread([this]{
            while (!Stop)
            {
                std::unique_lock<std::mutex> lock(Mutex);
                Condition.wait(lock, [this] { return Task != nullptr; });

                Task->Run();
            }
        });
    }

    PooledThread::PooledThread(PooledThread&& other) noexcept
        : Task(other.Task)
        , Thread(MoveTemp(other.Thread))
    {}

    void ThreadPool::Create(int32 threadNum)
    {
        if (threadNum <= 0)
        {
            return;
        }

        Workers.Reserve(threadNum);

        for (int32 idx = 0; idx < threadNum; ++idx)
        {
            //TODO: Name thread
            Workers.Add(PooledThread());
        }
    }

    void ThreadPool::Destroy()
    {
        Workers.Clear();
    }

    void ThreadPool::AddTask(const SharedPtr<IWorkThreadTask>& task)
    {
        TaskQueue.push(task);
    }
}