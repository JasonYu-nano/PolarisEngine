#pragma once

#include "graph_task.hpp"

namespace Engine
{
    class TASKFLOW_API Taskflow
    {
    public:
        using NodeIterator = Array<GraphTaskBase*>::Iterator;
        using ConstNodeIterator = Array<GraphTaskBase*>::ConstIterator;

        ~Taskflow()
        {
            Wait();
            Clear();
        }

        template <ConceptCallable Callable, typename... ArgTypes>
        GraphTask<Callable>& Add(ArgTypes&&... args)
        {
            GraphTask<Callable>* task = new GraphTask<Callable>(Forward<ArgTypes>(args)...);
            Add(task);
            return *task;
        }

        LambdaTask& Add(std::function<void()> lambda)
        {
            LambdaTask* task = new LambdaTask(MoveTemp(lambda));
            Add(task);
            return *task;
        }

        bool Empty() const
        {
            return Tasks.Empty();
        }

        int32 Size() const
        {
            return Tasks.Size();
        }

        bool IsExecutable() const { return Executable; }

        /**
         * Execute taskflow in multi thread.
         * One task flow can only execute once.
         */
        void Execute();

        /**
         * Block current thread until taskflow completed. It's thread unsafe
         * Can only block once.
         */
        void Wait();

        NodeIterator begin() { return Tasks.begin(); }

        ConstNodeIterator begin() const { return Tasks.begin(); }

        NodeIterator end() { return Tasks.end(); }

        ConstNodeIterator end() const { return Tasks.end(); }

    private:
        void Add(GraphTaskBase* newNode)
        {
            Tasks.Add(newNode);
        }

        void Clear();

        void OnLastTaskCompleted()
        {
            Promise.set_value();
        }

    private:
        bool PromiseRetrieved{ false };
        std::atomic<bool> Executable{ true };
        std::promise<void> Promise;
        Array<GraphTaskBase*> Tasks;
    };
}