#pragma once

#include <thread>
#include <condition_variable>
#include <queue>
#include <future>
#include "foundation/dynamic_array.hpp"
#include "definitions_core.hpp"

namespace Engine
{
    class CORE_API ThreadPool
    {
    public:
        ThreadPool(int32 threadNum);

        void Setup();
    private:
        std::atomic<bool> Terminated;
        std::condition_variable Event;
        std::mutex Mutex;
        DynamicArray<std::jthread> Workers;
        std::queue<std::packaged_task<void()>> TaskQueue;
    };
}
