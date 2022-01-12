#include "gtest/gtest.h"
#include "foundation/dynamic_array.hpp"
#include "memory/memory.hpp"
#include "foundation/sparse_array.hpp"
#include "foundation/stack.hpp"
#include "foundation/array.hpp"
#include "foundation/set.hpp"
#include "foundation/map.hpp"

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

    TEST(DynamicArrayTest, Base)
    {
        DynamicArray<int> array(10);
        EXPECT_TRUE(array.GetCapacity() == 10);

        array.Add(1);
        EXPECT_TRUE(array[0] == 1);

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
        EXPECT_TRUE(array.GetCount() == 2);

        array.RemoveMatch([](const int& item){
           return item > 0;
        });
        EXPECT_TRUE(array.IsEmpty());

        array.Clear(1);
        EXPECT_TRUE(array.IsEmpty() && array.GetCapacity() == 1);


        DynamicArray<ListTestStruct> listOfStruct(8);
        listOfStruct.Add(ListTestStruct());
        listOfStruct.Add(ListTestStruct());
        listOfStruct.Clear();

        DynamicArray<int> listOfCopy = { 1, 2, 3, 4, 0 };
        EXPECT_TRUE(listOfCopy[0] == 1 && listOfCopy[4] == 0);
        DynamicArray<int> listOfCopy2(4);
        listOfCopy2 = MoveTemp(listOfCopy);
        EXPECT_TRUE(listOfCopy2[0] == 1 && listOfCopy2[4] == 0);
    }

    TEST(DynamicArrayTest, OtherConstruct)
    {
        int rawData[] =  {1, 2, 3, 4};
        DynamicArray<int> array(rawData, 4);

        DynamicArray<int, FixedAllocator<int, 5>> fixedArray;
        EXPECT_TRUE(fixedArray.GetCapacity() == 5);
        fixedArray = {5, 4, 3, 2, 1};

        array = fixedArray;
        EXPECT_TRUE(array[0] == 5);
        EXPECT_TRUE(array[4] == 1);

        DynamicArray<int> array2 = {5, 4, 3, 2, 1};
        EXPECT_TRUE(array == array2);
    }

    TEST(DynamicArrayTest, Iterator)
    {
        DynamicArray<int> array = {1, 2, 3, 4, 0 };

        for (auto&& value : array)
        {
            PL_INFO("", TC("item of array is: {0}"), value);
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
            PL_INFO("", TC("item of array is: {0}"), *iter);
        }
    }

    TEST(BitArrayTest, Base)
    {
        BitArray array(10);
        EXPECT_TRUE(array.GetCapacity() == 32);
        array.Add(true);
        EXPECT_TRUE(array[0]);
        array[0] = false;
        EXPECT_TRUE(!array[0]);

        BitArray array2(true, 10);
        EXPECT_TRUE(array2.GetCount() == 10);
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

    TEST(BitArrayTest, Iterator)
    {
        BitArray array = {true, false, true, false, true, false};
        BitArray<>::Iterator iter = array.begin();
        (*iter) = false;
        EXPECT_TRUE(array[0] == false);
        for (BitRef value : array)
        {
            PL_INFO("", TC("item of bit array is {0}"), value);
        }

        for (auto iter = array.CreateValidIterator(); (bool)iter; ++iter)
        {
            PL_INFO("", TC("item of bit array is {0}"), *iter);
        }
    }

    TEST(SparseArrayTest, Base)
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

    TEST(SparseArrayTest, Iterator)
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

    TEST(SetTest, Base)
    {
        Set<int32> set;
        set.Add(1);
        set.Add(2);
        set.Add(-3);
        set.Add(2);

        Set<int32> set2 = MoveTemp(set);
        EXPECT_TRUE(set.GetCount() == 0);
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
            PL_INFO("", TC("destruct TestA"));
            Memory::Free(Ptr);
            Ptr = nullptr;
        }

        void* Ptr;

        friend uint32 GetHashCode(const TestA& inst)
        {
            return GetHashCode(reinterpret_cast<uint64>(inst.Ptr));
        }
    };

    TEST(SetTest, Ptr)
    {
        Set<TestA*> set;
        TestA* item = new TestA();
        set.Add(item);
        set.Add(item);
        EXPECT_TRUE(set.Contains(item));
    }

    TEST(SetTest, Iterator)
    {
        Set<int32> kSet{ 4, 6, 9, 3 };

        for (Set<int32>::ConstIterator iter = kSet.begin(); iter != kSet.end(); ++iter)
        {
            EXPECT_TRUE(kSet.Contains(*iter));
        }
    }

    TEST(MapTest, Base)
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
        EXPECT_TRUE(map.GetCount() == 0 && map2.GetCount() == 1);
        map2.Clear(2);
        EXPECT_TRUE(map2.GetCount() == 0);
    }

    TEST(MapTest, Iterator)
    {
        Map<int32, float> map = {{1, 1.5f}, {2, 2.5f}, {3, 1.6f}};
        for (const auto& pair : map)
        {
            PL_INFO("", TC("key:{0} value:{1}"), pair.Key, pair.Value);
        }

        for (Map<int32, float>::ConstIterator iter = map.begin(); iter != map.end(); ++iter)
        {
            PL_INFO("", TC("key:{0} value:{1}"), iter->Key, iter->Value);
        }
    }
}