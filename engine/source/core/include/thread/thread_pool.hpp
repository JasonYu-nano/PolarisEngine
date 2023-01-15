#pragma once

#include <thread>
#include <condition_variable>
#include <queue>
#include <future>
#include "foundation/array.hpp"
#include "definitions_core.hpp"
#include "foundation/smart_ptr.hpp"

namespace Engine
{
    class IThreadPool;

    class CORE_API IWorkThreadTask
    {
    public:
        virtual ~IWorkThreadTask() = default;

        virtual void Run() = 0;
    };

    class WorkThread
    {
    public:
        explicit WorkThread(IThreadPool* owner);

        WorkThread(WorkThread&& other) noexcept;

    public:
        std::atomic<bool> Stop;
        IWorkThreadTask* volatile Task{ nullptr };
        std::condition_variable Condition;

    private:
        IThreadPool* Owner;
        std::jthread Thread;
    };

    class CORE_API IThreadPool
    {
    public:
        IThreadPool() = default;

        virtual ~IThreadPool() = default;

        virtual void Create(int32 threadNum) = 0;

        virtual void Destroy() = 0;

        virtual void AddTask(IWorkThreadTask* task) = 0;

        virtual IWorkThreadTask* GetNextTask(WorkThread& worker) = 0;
    };

    class CORE_API BuiltInThreadPool : public IThreadPool
    {
    public:
        BuiltInThreadPool() = default;

        ~BuiltInThreadPool() override { DestroyInternal(); }

        void Create(int32 threadNum) override;

        void Destroy() override
        {
            DestroyInternal();
        }

        void AddTask(IWorkThreadTask* task) override;

        IWorkThreadTask* GetNextTask(WorkThread& worker) override;
    private:
        void DestroyInternal();

        Array<WorkThread*> AllWorkers;
        Array<WorkThread> TestAllWorkers;
        std::queue<IWorkThreadTask*> TaskQueue;
        std::queue<WorkThread*> IdleWorkers;
        std::mutex TaskQueueMutex;
        std::mutex WorkerQueueMutex;
    };
}
