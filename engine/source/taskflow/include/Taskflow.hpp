#pragma once

#include "Graph.hpp"
#include "Task.hpp"

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

    private:
        //TODO: Cache graph per thread
        Graph TaskGraph;
    };
}