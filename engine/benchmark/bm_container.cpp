#include "benchmark/benchmark.h"
#include "foundation/dynamic_array.hpp"
#include "foundation/set.hpp"
#include <vector>
#include <set>
#include <unordered_set>
#include <iostream>

using namespace Engine;

static void BM_DyanmicArrayAdd(benchmark::State& state)
{
    DynamicArray<uint32> array;
    for (auto _ : state) 
    {
        for (uint32 i = 0; i < 1000; i++)
        {
            array.Add(i);
        }
        array.Clear();
        array.Resize(0);
    }
}

static void BM_VectorAdd(benchmark::State& state)
{
    std::vector<uint32> array;
    for (auto _ : state) 
    {
        for (uint32 i = 0; i < 1000; i++)
        {
            array.push_back(i);
        }
        array.clear();
        array.resize(0);
    }
}

static void BM_DyanmicArrayRemove(benchmark::State& state)
{
    DynamicArray<uint32> array;
    for (auto _ : state) 
    {
        for (uint32 i = 0; i < 1000; i++)
        {
            array.Add(i);
        }

        while (array.GetCount() > 0)
        {
            array.RemoveAt(0);
        }

        array.Resize(0);
    }
}

static void BM_VectorRemove(benchmark::State& state)
{
    std::vector<uint32> array;
    for (auto _ : state) 
    {
        for (uint32 i = 0; i < 1000; i++)
        {
            array.push_back(i);
        }

        while (array.size() > 0)
        {
            array.erase(array.begin());
        }

        array.resize(0);
    }
}

static void BM_DyanmicArrayLoop(benchmark::State& state)
{
    DynamicArray<uint32> array;
    for (uint32 i = 0; i < 1000; i++)
    {
        array.Add(i);
    }
    for (auto _ : state) 
    {
        for (auto _ : array)
        {

        }
    }
}

static void BM_VectorLoop(benchmark::State& state)
{
    std::vector<uint32> array;
    for (uint32 i = 0; i < 1000; i++)
    {
        array.push_back(i);
    }
    for (auto _ : state) 
    {
        for (auto _ : array)
        {

        }
    }
}

static void BM_SetAdd(benchmark::State& state)
{
    for (auto _ : state)
    {
        Set<uint32> mySet;
        for (uint32 i = 0; i < 1000; i++)
        {
            mySet.Add(i);
        }
    }
}

static void BM_StlHashSetAdd(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::unordered_set<uint32> mySet;
        for (uint32 i = 0; i < 1000; i++)
        {
            mySet.insert(i);
        }
    }
}

static void BM_StlSetAdd(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::set<uint32> mySet;
        for (uint32 i = 0; i < 1000; i++)
        {
            mySet.insert(i);
        }
    }
}

static void BM_SetLoop(benchmark::State& state)
{
    Set<uint32> mySet;
    for (uint32 i = 1000; i > 0; i--)
    {
        mySet.Add(i);
    }

    uint32 v = 0;
    for (auto _ : state)
    {
        for (auto&& item : mySet)
        {
            v = item;
        }
    }
    std::cout << v;
}

static void BM_StlHashSetLoop(benchmark::State& state)
{
    std::unordered_set<uint32> mySet;
    for (uint32 i = 1000; i > 0; i--)
    {
        mySet.insert(i);
    }

    uint32 v = 0;
    for (auto _ : state)
    {
        for (auto&& item : mySet)
        {
            v = item;
        }
    }
    std::cout << v;
}

static void BM_StlSetLoop(benchmark::State& state)
{
    std::set<uint32> mySet;
    for (uint32 i = 1000; i > 0; i--)
    {
        mySet.insert(i);
    }

    uint32 v = 0;
    for (auto _ : state)
    {
        for (auto&& item : mySet)
        {
            v = item;
        }
    }
    std::cout << v;
}

BENCHMARK(BM_DyanmicArrayAdd);
BENCHMARK(BM_VectorAdd);

BENCHMARK(BM_DyanmicArrayRemove);
BENCHMARK(BM_VectorRemove);

BENCHMARK(BM_DyanmicArrayLoop);
BENCHMARK(BM_VectorLoop);

BENCHMARK(BM_SetAdd);
BENCHMARK(BM_StlHashSetAdd);
BENCHMARK(BM_StlSetAdd);

BENCHMARK(BM_SetLoop);
BENCHMARK(BM_StlHashSetLoop);
BENCHMARK(BM_StlSetLoop);

BENCHMARK_MAIN();