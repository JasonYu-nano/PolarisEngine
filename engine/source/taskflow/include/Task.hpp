#pragma once

#include "GraphNode.hpp"

namespace Engine
{
    template <typename T>
    concept ConceptCallable = requires(T a)
    {
        T();
    };

    template <ConceptCallable CallableType>
    class Task : public GraphNode
    {
        friend class Taskflow;
    public:
        Task() = delete;

    private:
        explicit Task(const CallableType& callable) : Callable(callable) {}
        CallableType Callable;
    };
}