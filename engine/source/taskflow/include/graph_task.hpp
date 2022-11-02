#pragma once

#include "graph_node.hpp"

namespace Engine
{
    class GraphTaskBase
    {
    public:
        virtual ~GraphTaskBase() = default;

        virtual void Precede(GraphTaskBase* node)
        {
            if (node != this)
            {
                Prerequisites.AddUnique(node);
                node->Dependencies.Add(this);
                ++WaitingPrerequisites;
            }
        }

        int32 DependencyNum() const { return Dependencies.Size(); }

        virtual void ConditionDispatch() const = 0;

        GraphTaskBase& operator-- ()
        {
            return *this;
        }

        GraphTaskBase& operator> (GraphTaskBase* node)
        {
            node->Precede(this);
            return *this;
        }

        GraphTaskBase& operator< (GraphTaskBase* node)
        {
            Precede(node);
            return *this;
        }

    protected:
        std::atomic<int32> WaitingPrerequisites;
        DynamicArray<GraphTaskBase*> Prerequisites;
        DynamicArray<GraphTaskBase*> Dependencies;
    };

    template <typename T>
    concept ConceptCallable = requires(T a)
    {
        T();
    };

    template <ConceptCallable CallableType>
    class GraphTask : public GraphTaskBase
    {
        using Super = GraphNode;
        friend class Taskflow;
    public:
        template <typename... ArgTypes>
        GraphTask(ArgTypes&&... args)
            : Callable(CallableType(Forward<ArgTypes>(args)...))
        {}

        void ConditionDispatch() const override
        {
            if (WaitingPrerequisites > 0)
            {
                return;
            }
            // TODO: Dispatch task
        }

        void Run()
        {
            Callable();

            for (GraphTaskBase* child : Dependencies)
            {
                child->ConditionDispatch();
            }
        }

    private:
        explicit GraphTask(const CallableType& callable) : Callable(callable) {}
        CallableType Callable;
    };
}