#pragma once

#include "graph.hpp"
#include "task.hpp"

namespace Engine
{
    class TASKFLOW_API Taskflow
    {
    public:
        template <ConceptCallable Callable>
        Task<Callable>& Add(const Callable& callable)
        {
            Task<Callable>* task = new Task<Callable>(callable);
            TaskGraph.Add(task);
            return *task;
        }

        const Graph& GetGraph() const { return TaskGraph; }

    private:
        //TODO: Cache Graph per thread
        Graph TaskGraph;
    };
}