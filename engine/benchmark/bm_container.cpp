#include "benchmark/benchmark.h"
#include "foundation/dynamic_array.hpp"
#include "foundation/set.hpp"
#include "foundation/ustring.hpp"
#include "math/quaternion.hpp"
#include <vector>
#include <set>
#include <unordered_set>

using namespace Engine;

static void BM_DyanmicArrayAdd(benchmark::State& state)
{
    DynamicArray<int32> array;
    for (auto _ : state) 
    {
        for (int32 i = 0; i < 1000; i++)
        {
            array.Add(i);
        }
        array.Clear();
        array.Resize(0);
    }
}

static void BM_VectorAdd(benchmark::State& state)
{
    std::vector<int32> array;
    for (auto _ : state) 
    {
        for (int32 i = 0; i < 1000; i++)
        {
            array.push_back(i);
        }
        array.clear();
        array.resize(0);
    }
}

static void BM_DyanmicArrayRemove(benchmark::State& state)
{
    DynamicArray<int32> array;
    for (int32 i = 0; i < 1000; i++)
    {
        array.Add(i);
    }

    for (auto _ : state) 
    {
        auto copy = array;

        while (copy.Size() > 0)
        {
            copy.RemoveAt(0);
        }
    }
}

static void BM_VectorRemove(benchmark::State& state)
{
    std::vector<int32> array;
    for (int32 i = 0; i < 1000; i++)
    {
        array.push_back(i);
    }

    for (auto _ : state) 
    {
        auto copy = array;

        while (!copy.empty())
        {
            copy.erase(copy.begin());
        }
    }
}

static void BM_DyanmicArrayLoop(benchmark::State& state)
{
    DynamicArray<int32> array;
    for (int32 i = 0; i < 10000; i++)
    {
        array.Add(i);
    }
    int64 count = 0;
    for (auto _ : state)
    {
        for (auto&& val : array)
        {
            count += val;
        }
    }

    printf("%lld\n", count);
}

static void BM_VectorLoop(benchmark::State& state)
{
    std::vector<int32> array;
    for (int32 i = 0; i < 10000; i++)
    {
        array.push_back(i);
    }
    int64 count = 0;
    for (auto _ : state) 
    {
        for (auto&& val : array)
        {
            count += val;
        }
    }

    printf("%lld\n", count);
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
}

static void BM_StdString(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::string str = "abcdefg";
        if (str != "abcdefg")
        {
            continue;
        }
    }
}

static void BM_UString(benchmark::State& state)
{
    for (auto _ : state)
    {
        UString str = "abcdefg";
        if (str != "abcdefg")
        {
            break;
        }
    }
}

static void BM_StdStringAppend(benchmark::State& state)
{
    std::string str = "";
    for (auto _ : state)
    {
        str.append("1");
        if (str.length() > 0)
        {
            continue;
        }
    }
}

static void BM_UStringAppend(benchmark::State& state)
{
    std::string str2 = "";
    UString str = "";
    for (auto _ : state)
    {
        str.Append("1");
        if (str.Length())
        {
            continue;
        }
    }
}

static void BM_UStringFromUtf8(benchmark::State& state)
{
    for (auto _ : state)
    {
        UString::FromUtf8("abcdedg");
    }
}

static void BM_UStringFromLatin1(benchmark::State& state)
{
    for (auto _ : state)
    {
        UString::FromLatin1("abcdedg");
    }
}

static void BM_RotatorToQuat(benchmark::State& state)
{
    Rotator rotator(10, 60, 10);
    for (auto _ : state)
    {
        rotator.ToQuaternion();
    }
}

static void BM_VectorCross(benchmark::State& state)
{
    Vector3f vec(5, 6.2, 7.1);
    for (auto _ : state)
    {
        Vector3f::Cross(vec, vec);
    }
}

static void BM_QuatNormalize(benchmark::State& state)
{
    Quat quat({5, 4, 3, 1});
    for (auto _ : state)
    {
        quat.Normalize();
    }
}

//BENCHMARK(BM_DyanmicArrayAdd);
//BENCHMARK(BM_VectorAdd);
//
//BENCHMARK(BM_DyanmicArrayRemove);
//BENCHMARK(BM_VectorRemove);
//
//BENCHMARK(BM_DyanmicArrayLoop);
//BENCHMARK(BM_VectorLoop);

//BENCHMARK(BM_SetAdd);
//BENCHMARK(BM_StlHashSetAdd);
//BENCHMARK(BM_StlSetAdd);
//
//BENCHMARK(BM_SetLoop);
//BENCHMARK(BM_StlHashSetLoop);
//BENCHMARK(BM_StlSetLoop);

//BENCHMARK(BM_StdString);
//BENCHMARK(BM_UString);
//
//BENCHMARK(BM_StdStringAppend);
//BENCHMARK(BM_UStringAppend);
//
//BENCHMARK(BM_UStringFromUtf8);
//BENCHMARK(BM_UStringFromLatin1);

BENCHMARK(BM_RotatorToQuat);
BENCHMARK(BM_VectorCross);
BENCHMARK(BM_QuatNormalize);

BENCHMARK_MAIN();