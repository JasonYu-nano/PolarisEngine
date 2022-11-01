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
    class CORE_API IWorkThreadTask
    {
    public:
        virtual ~IWorkThreadTask() = default;

        virtual void Run() = 0;
    };

    class PooledThread
    {
    public:
        PooledThread();

        PooledThread(PooledThread&& other) noexcept ;

        void operator()();

    private:
        IWorkThreadTask* Task{ nullptr };
        std::jthread Thread;
        std::mutex Mutex;
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

        virtual void AddTask(const SharedPtr<IWorkThreadTask>& task);
    private:
        DynamicArray<PooledThread> Workers;
        std::queue<SharedPtr<IWorkThreadTask>> TaskQueue;
    };
}
