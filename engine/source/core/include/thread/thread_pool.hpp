#pragma once

#include <thread>
#include <condition_variable>
#include <queue>
#include <future>
#include "foundation/dynamic_array.hpp"
#include "definitions_core.hpp"
#include "foundation/smart_ptr.hpp"

namespace Engine
{
    class ThreadPool;

    class CORE_API IWorkThreadTask
    {
    public:
        virtual ~IWorkThreadTask() = default;

        virtual void Run() = 0;
    };

    class WorkThread
    {
        friend class ThreadPool;
    public:
        WorkThread(ThreadPool& owner);

        WorkThread(WorkThread&& other) noexcept ;

        void operator()();

    private:
        ThreadPool& Owner;
        IWorkThreadTask* volatile Task{ nullptr };
        std::jthread Thread;
        std::condition_variable Condition;
        std::atomic<bool> Stop;
    };

    class CORE_API ThreadPool
    {
    public:
        ThreadPool() = default;

        virtual ~ThreadPool() { Destroy(); }

        void Create(int32 threadNum);

        void Destroy();

        virtual void AddTask(IWorkThreadTask* task);

        virtual IWorkThreadTask* GetNextTask(WorkThread& worker);
    private:
        DynamicArray<WorkThread*> AllWorkers;
        DynamicArray<WorkThread> TestAllWorkers;
        std::queue<IWorkThreadTask*> TaskQueue;
        std::queue<WorkThread*> IdleWorkers;
        std::mutex TaskQueueMutex;
        std::mutex WorkerQueueMutex;
    };
}
