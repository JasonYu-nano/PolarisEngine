#include "thread/thread_pool.hpp"

namespace Engine
{
    ThreadPool::ThreadPool(int32 threadNum)
    {
        if (threadNum <= 0)
        {
            return;
        }

        Workers.Reserve(threadNum);
    }

    void ThreadPool::Setup()
    {
        for (int32 idx = 0; idx < Workers.Capacity(); ++idx)
        {
            //TODO: Name thread
            Workers.Add(std::jthread([this]() {
                while (true)
                {
                    std::packaged_task<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(Mutex);
                        Event.wait(lock, [this]{
                            return !TaskQueue.empty();
                        });

                        task = MoveTemp(TaskQueue.front());
                        TaskQueue.pop();
                    }
                    task();
                }
            }));
        }
    }
}