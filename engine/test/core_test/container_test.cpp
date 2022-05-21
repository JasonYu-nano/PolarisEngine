#include "gtest/gtest.h"
#include "foundation/dynamic_array.hpp"
#include "memory/memory.hpp"
#include "foundation/sparse_array.hpp"
#include "foundation/set.hpp"
#include "foundation/map.hpp"
#include <vector>

namespace Engine
{
    struct ListTestStruct
    {
        ~ListTestStruct()
        {
            if (nullptr != Z)
            {
                delete Z;
                Z = nullptr;
            }
        }
        bool X{ false };
        int32 Y{ 0 };
        int32* Z{ nullptr };
    };

    TEST(ContainerTest, DynamicArray_Base)
    {
        DynamicArray<int> array(10);
        EXPECT_TRUE(array.Capacity() == 10);

        array.Add(1);
        EXPECT_TRUE(array[0] == 1);

        int part[3] = {2, 3, 4};
        array.Add(part, 3);

        EXPECT_TRUE(array.Size() == 4);
        EXPECT_TRUE(array[1] == 2 && array[2] == 3 && array[3] == 4);

        array = {1};

        array[0] = 10;
        EXPECT_TRUE(array[0] == 10);

        array.RemoveAt(0);
        EXPECT_TRUE(array.IsEmpty());

        int& item = array.AddDefault();
        item = 10;
        array.Insert(0, 9);
        array.Insert(2, 11);
        EXPECT_TRUE(array[0] == 9 && array[1] == 10 && array[2] == 11);

        array.Remove(10);
        EXPECT_TRUE(array[0] == 9 && array[1] == 11);
        EXPECT_TRUE(array.Size() == 2);

        array.RemoveMatch([](const int& item){
           return item > 0;
        });
        EXPECT_TRUE(array.IsEmpty());

        array.Clear(1);
        EXPECT_TRUE(array.IsEmpty() && array.Capacity() == 1);


        DynamicArray<ListTestStruct> listOfStruct(8);
        listOfStruct.Add(ListTestStruct());
        listOfStruct.Add(ListTestStruct());
        listOfStruct.Clear();

        DynamicArray<int> listOfCopy = { 1, 2, 3, 4, 0 };
        EXPECT_TRUE(listOfCopy[0] == 1 && listOfCopy[4] == 0);
        DynamicArray<int> listOfCopy2(4);
        listOfCopy2 = MoveTemp(listOfCopy);
        EXPECT_TRUE(listOfCopy2[0] == 1 && listOfCopy2[4] == 0);

        DynamicArray<int> array2 = {1, 2, 3};
        array2.Resize(5);
        EXPECT_TRUE(array2[4] == 0 && array2.Size() == 5);

        array2.Resize(3);
        array2.Resize(5, 4);
        EXPECT_TRUE(array2[4] == 4);

        int32 oldCapacity = array2.Capacity();
        array2.Resize(0);
        EXPECT_TRUE(array2.Capacity() == oldCapacity);
    }

    TEST(ContainerTest, DynamicArray_Ctor)
    {
        int rawData[] =  {1, 2, 3, 4};
        DynamicArray<int> array(rawData, 4);

        DynamicArray<int, FixedAllocator<int, 5>> fixedArray;
        EXPECT_TRUE(fixedArray.Capacity() == 5);
        fixedArray = {5, 4, 3, 2, 1};

        array = fixedArray;
        EXPECT_TRUE(array[0] == 5);
        EXPECT_TRUE(array[4] == 1);

        DynamicArray<int> array2 = {5, 4, 3, 2, 1};
        EXPECT_TRUE(array == array2);

        DynamicArray<int, InlineAllocator<int, 5, HeapAllocator<int>, int>> inlineArray = {5, 4, 3, 2, 1};
        EXPECT_TRUE(inlineArray.Capacity() == 5);
    }

    TEST(ContainerTest, DynamicArray_Remove)
    {
        DynamicArray<int32> array = {1, 2, 3, 4, 5};
        array.Remove(4, 4);
        EXPECT_TRUE(array.Size() == 4 && *(--array.end()) == 4);

        array.Add(5);
        array.Remove(1, 3);
        EXPECT_TRUE(array.Size() == 2 && *(--array.end()) == 5 && *array.begin() == 1);
        array.Remove(0, 1);
        EXPECT_TRUE(array.Size() == 0);
    }

    TEST(ContainerTest, DynamicArray_Iterator)
    {
        DynamicArray<int> array = {1, 2, 3, 4, 0 };

        for (auto&& value : array)
        {
            PL_INFO("", _T("item of array is: {0}"), value);
        }

        for (DynamicArray<int>::Iterator iter = array.begin(); iter != array.end(); ++iter)
        {
            if (*iter > 2)
            {
                iter.RemoveSelf();
            }
        }

        for (DynamicArray<int>::ConstIterator iter = array.begin(); iter != array.end(); ++iter)
        {
            PL_INFO("", _T("item of array is: {0}"), *iter);
        }
    }

    TEST(ContainerTest, BitArray_Base)
    {
        BitArray array(10);
        EXPECT_TRUE(array.Capacity() == 32);
        array.Add(true);
        EXPECT_TRUE(array[0]);
        array[0] = false;
        EXPECT_TRUE(!array[0]);

        BitArray array2(true, 10);
        EXPECT_TRUE(array2.Size() == 10);
        EXPECT_TRUE(array2[0] == true);
        EXPECT_TRUE(array2[9] == true);

        BitArray array3{true, true, false};
        array3.Insert(1, false);
        EXPECT_TRUE(array3[1] == false && array3[2] == true && array3[3] == false);
        array3 = {false};
        EXPECT_TRUE(array3[0] == false);
        EXPECT_TRUE(array3.GetElementCount() == 1);

        BitArray array4(array3);
        EXPECT_TRUE(array3 == array4);
    }

    TEST(ContainerTest, BitArray_Iterator)
    {
        BitArray array = {true, false, true, false, true, false};
        BitArray<>::Iterator iter = array.begin();
        (*iter) = false;
        EXPECT_TRUE(array[0] == false);
        for (BitRef value : array)
        {
            PL_INFO("", _T("item of bit array is {0}"), value);
        }

        for (auto iter = array.CreateValidIterator(); (bool)iter; ++iter)
        {
            PL_INFO("", _T("item of bit array is {0}"), *iter);
        }
    }

    TEST(ContainerTest, SparseArray_Base)
    {
        SparseArray<int32> array(8);
        uint32 index1 = array.Add(-20);
        uint32 index2 = array.Add(11);
        EXPECT_TRUE(index1 == 0 && index2 && 1);
        array.Add(15);
        array.Add(-1); //{-20, 11, 15, -1}
        array.RemoveAt(0); //{null, 11, 15, -1}
        array.Add(6); //{6, 11, 15, -1}
        EXPECT_TRUE(array[0] == 6 && array[1] == 11);
        array.RemoveAt(2); //{6, 11, null, -1}

        SparseArray<int32> array2 = {6, 11, -1};
        EXPECT_FALSE(array == array2);

        array2.Insert(3, -1); //{6, 11, -1, -1}
        array2.RemoveAt(2); //{6, 11, null, -1}

        EXPECT_TRUE(array == array2);
    }

    TEST(ContainerTest, SparseArray_Iterator)
    {
        SparseArray<int32> array(8);
        uint32 index1 = array.Add(-20);
        uint32 index2 = array.Add(11);
        array.Add(15);
        array.Add(-1); //{-20, 11, 15, -1}


        for (auto&& value : array)
        {
        }

        for (SparseArray<int32>::ConstIterator iter = array.begin(); iter != array.end(); ++iter)
        {
        }
    }

    TEST(ContainerTest, Set_Base)
    {
        Set<int32> set;
        set.Add(1);
        set.Add(2);
        set.Add(-3);
        set.Add(2);

        Set<int32> set2 = MoveTemp(set);
        EXPECT_TRUE(set.Size() == 0);
        EXPECT_TRUE(set2.Contains(1));
        EXPECT_TRUE(set2.Contains(2));
        EXPECT_TRUE(set2.Contains(-3));
        EXPECT_FALSE(set2.Contains(0));

        set2.Remove(2);
        EXPECT_TRUE(set2.Contains(1));
        set2.Remove(-3);
        EXPECT_TRUE(set2.Contains(1));
    }

    struct TestA
    {
        TestA()
                : Ptr(Memory::Malloc(100))
        {}

        ~TestA()
        {
            PL_INFO("", _T("destruct TestA"));
            Memory::Free(Ptr);
            Ptr = nullptr;
        }

        void* Ptr;

        friend uint32 GetHashCode(const TestA& inst)
        {
            return GetHashCode(reinterpret_cast<uint64>(inst.Ptr));
        }
    };

    TEST(ContainerTest, Set_Ptr)
    {
        Set<TestA*> set;
        TestA* item = new TestA();
        set.Add(item);
        set.Add(item);
        EXPECT_TRUE(set.Contains(item));
    }

    TEST(ContainerTest, Set_Iterator)
    {
        Set<int32> kSet{ 4, 6, 9, 3 };

        for (Set<int32>::ConstIterator iter = kSet.begin(); iter != kSet.end(); ++iter)
        {
            EXPECT_TRUE(kSet.Contains(*iter));
        }
    }

    TEST(ContainerTest, Map_Base)
    {
        Map<int32, float> map = {{1, 1.5f}, {2, 2.5f}, {1, 1.6f}};
        EXPECT_TRUE(*(map.Find(1)) == 1.6f);
        EXPECT_TRUE(*(map.Find(2)) == 2.5f);

        float& v = map.FindOrAdd(1, 1.8f);
        EXPECT_TRUE(v == 1.6f);

        map.Remove(1111);
        map.Remove(1);
        EXPECT_TRUE(map.Find(1) == nullptr);

        Map<int32, float> map2 = MoveTemp(map);
        EXPECT_TRUE(*map2.Find(2) == 2.5f);
        EXPECT_TRUE(map.Size() == 0 && map2.Size() == 1);
        map2.Clear(2);
        EXPECT_TRUE(map2.Size() == 0);
    }

    TEST(ContainerTest, Map_Iterator)
    {
        Map<int32, float> map = {{1, 1.5f}, {2, 2.5f}, {3, 1.6f}};
        for (const auto& pair : map)
        {
            PL_INFO("", _T("key:{0} value:{1}"), pair.Key, pair.Value);
        }

        for (Map<int32, float>::ConstIterator iter = map.begin(); iter != map.end(); ++iter)
        {
            PL_INFO("", _T("key:{0} value:{1}"), iter->Key, iter->Value);
        }
    }
}