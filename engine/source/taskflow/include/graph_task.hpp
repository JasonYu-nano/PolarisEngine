#pragma once

#include "thread/thread_pool.hpp"
#include "definitions_taskflow.hpp"

namespace Engine
{
    class TASKFLOW_API GraphTaskBase : public IWorkThreadTask
    {
    public:
        ~GraphTaskBase() override = default;

        virtual void Precede(GraphTaskBase* node);

        int32 DependencyNum() const { return Prerequisites.Size(); }

        virtual void ConditionDispatch();

        void Run() override = 0;

    protected:
        std::atomic<int32> WaitingPrerequisites;
        Array<GraphTaskBase*> Prerequisites;
        Array<GraphTaskBase*> Subsequences;
    };

    TASKFLOW_API inline GraphTaskBase& operator --(GraphTaskBase& task, int)
    {
        return task;
    }

    TASKFLOW_API inline GraphTaskBase& operator> (GraphTaskBase& pre, GraphTaskBase& depend)
    {
        pre.Precede(&depend);
        return depend;
    }

    template <typename T>
    concept ConceptCallable = requires(T a)
    {
        a();
    };

    template <ConceptCallable CallableType>
    class GraphTask : public GraphTaskBase
    {
        using Super = GraphTaskBase;
        friend class Taskflow;
    public:
        template <typename... ArgTypes>
        GraphTask(ArgTypes&&... args)
            : Callable(CallableType(Forward<ArgTypes>(args)...))
        {}

        void Run() override
        {
            Callable();

            for (GraphTaskBase* child : Subsequences)
            {
                child->ConditionDispatch();
            }
        }

    private:
        explicit GraphTask(const CallableType& callable) : Callable(callable) {}
        CallableType Callable;
    };

    class TASKFLOW_API LambdaTask : public GraphTaskBase
    {
    public:
        explicit LambdaTask(std::function<void()> lambda) : Lambda(MoveTemp(lambda)) {}

        void Run() override
        {
            Lambda();

            for (GraphTaskBase* child : Subsequences)
            {
                child->ConditionDispatch();
            }
        }

    private:
        std::function<void()> Lambda;
    };
}