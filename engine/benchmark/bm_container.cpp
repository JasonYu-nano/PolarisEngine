#include "benchmark/benchmark.h"
#include "foundation/dynamic_array.hpp"
#include <vector>

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
        auto iter = array.begin();
        while (iter != array.end())
        {
            (*iter)++;
            ++iter;
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
        auto iter = array.begin();
        while (iter != array.end())
        {
            (*iter)++;
            ++iter;
        }
    }
}

BENCHMARK(BM_DyanmicArrayAdd);
BENCHMARK(BM_VectorAdd);

BENCHMARK(BM_DyanmicArrayRemove);
BENCHMARK(BM_VectorRemove);

BENCHMARK(BM_DyanmicArrayLoop);
BENCHMARK(BM_VectorLoop);

BENCHMARK_MAIN();