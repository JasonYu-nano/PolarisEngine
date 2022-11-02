#pragma once

#include "graph.hpp"
#include "graph_task.hpp"

namespace Engine
{
    class TASKFLOW_API Taskflow
    {
    public:
    public:
        using NodeIterator = DynamicArray<GraphTaskBase*>::Iterator;
        using ConstNodeIterator = DynamicArray<GraphTaskBase*>::ConstIterator;

        ~Taskflow()
        {
            Clear();
        }

        template <ConceptCallable Callable, typename... ArgTypes>
        GraphTask<Callable>& Add(ArgTypes&&... args)
        {
            GraphTask<Callable>* task = new GraphTask<Callable>(Forward<ArgTypes>(args)...);
            Add(task);
            return *task;
        }

        void Add(GraphTaskBase* newNode)
        {
            Tasks.Add(newNode);
        }

        void Clear()
        {
            for (GraphTaskBase* node : Tasks)
            {
                delete node;
            }
            Tasks.Clear();
        }

        bool Empty() const
        {
            return Tasks.IsEmpty();
        }

        int32 Size() const
        {
            return Tasks.Size();
        }

        NodeIterator begin() { return Tasks.begin(); }

        ConstNodeIterator begin() const { return Tasks.begin(); }

        NodeIterator end() { return Tasks.end(); }

        ConstNodeIterator end() const { return Tasks.end(); }

    private:
        DynamicArray<GraphTaskBase*> Tasks;
    };
}