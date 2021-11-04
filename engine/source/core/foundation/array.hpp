#pragma once

#include <vector>
#include <span>

template<typename Type>
using Vector = std::vector<Type>;

template<typename T, size_t MaxSize>
using Array = std::array<T, MaxSize>;

template<typename T>
using Span = std::span<T>;

namespace Engine
{
    template<typename Element, typename Allocator> 
    class TArray
    {
    public:
        TArray() 
            : Count(0)
            , Max(0)
        {}

        ~TArray() = default;
    private:
        uint32 Count;
        uint32 Max;
        Allocator* AllocatorInstance;
    };
}
