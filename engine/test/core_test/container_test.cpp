#include "gtest/gtest.h"
#include "foundation/dynamic_array.hpp"
#include "memory/memory.hpp"
#include "foundation/sparse_array.hpp"
#include "foundation/set.hpp"
#include "foundation/map.hpp"
#include "log/logger.hpp"
#include "foundation/array.hpp"
#include <vector>

namespace Engine
{
    struct ListTestStruct
    {
        ListTestStruct() = default;
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

    struct NonTrivialArrayItem
    {
        NonTrivialArrayItem()
        {
            ++AllocCounter;
            ValuePtr = new int32(0);
        }

        NonTrivialArrayItem(int32 val)
        {
            ++AllocCounter;
            ValuePtr = new int32(val);
        }

        NonTrivialArrayItem(const NonTrivialArrayItem& other)
        {
            ++AllocCounter;
            ValuePtr = new int32(*other.ValuePtr);
        }

        ~NonTrivialArrayItem()
        {
            --AllocCounter;
            delete ValuePtr;
            ValuePtr = nullptr;
        }

        bool operator== (const NonTrivialArrayItem& other) const
        {
            return *ValuePtr == *other.ValuePtr;
        }

        bool operator!= (const NonTrivialArrayItem& other) const
        {
            return *ValuePtr != *other.ValuePtr;
        }

        static int32 AllocCounter;
        int32* ValuePtr{ nullptr };
    };

    int32 NonTrivialArrayItem::AllocCounter = 0;

    TEST(ContainerTest, Array_Ctor)
    {
        Array<int32> array1;
        EXPECT_TRUE(array1.Size() == 0 && array1.Capacity() == 0 && array1.Data() == nullptr);

        Array<int32> array2(10);
        EXPECT_TRUE(array2.Capacity() == 10);

        int32 numbers[5] = {0, 1, 2, 3, 4};
        Array<int32> array3(numbers, 5);
        for (int32 i = 0; i < 5; ++i)
        {
            EXPECT_TRUE(array3[i] == i);
        }

        Array<int32> array4(0, 10);
        for (int32 i = 0; i < 10; ++i)
        {
            EXPECT_TRUE(array4[i] == 0);
        }

        Array<int32> array5 = {0, 1, 2, 3, 4};
        for (int32 i = 0; i < 5; ++i)
        {
            EXPECT_TRUE(array5[i] == i);
        }

        Array<int32> array6 = array5;
        for (int32 i = 0; i < 5; ++i)
        {
            EXPECT_TRUE(array6[i] == array5[i]);
        }

        Array<int32> array7 = std::move(array6);
        for (int32 i = 0; i < 5; ++i)
        {
            EXPECT_TRUE(array7[i] == array5[i]);
        }
        EXPECT_TRUE(array6.Size() == 0 && array6.Capacity() == 0 && array6.Data() == nullptr);

        EXPECT_TRUE(std::is_trivial_v<NonTrivialArrayItem> == false);
        Array<NonTrivialArrayItem> array8(NonTrivialArrayItem(), 5);
        EXPECT_TRUE(NonTrivialArrayItem::AllocCounter == 5);
        Array<NonTrivialArrayItem> array9(array8);
        EXPECT_TRUE(NonTrivialArrayItem::AllocCounter == 10);
        array8.Clear();
        EXPECT_TRUE(NonTrivialArrayItem::AllocCounter == 5);

        array9 = {NonTrivialArrayItem(0), NonTrivialArrayItem(1)};
        EXPECT_TRUE(NonTrivialArrayItem::AllocCounter == 2);

        Array<NonTrivialArrayItem> array10 = {NonTrivialArrayItem(2), NonTrivialArrayItem(3)};
        array9 = array10;
        EXPECT_TRUE(NonTrivialArrayItem::AllocCounter == 4);

        array9 = std::move(array10);
        EXPECT_TRUE(NonTrivialArrayItem::AllocCounter == 2);
    }

    TEST(ContainerTest, Array_Add)
    {
        Array<NonTrivialArrayItem> array;
        auto item = NonTrivialArrayItem(0);
        array.Add(item);
        array.Add({1});

        NonTrivialArrayItem items[2] = {NonTrivialArrayItem(2), NonTrivialArrayItem(3)};
        array.Add(items, 2);

        for (int32 i = 0; i < 4; ++i)
        {
            EXPECT_TRUE(*array[i].ValuePtr == i);
        }

        array.AddDefault();
        EXPECT_TRUE(*array.Last().ValuePtr == 0);
        *array.Last().ValuePtr = 4;

        EXPECT_TRUE(array.AddUnique(item) == false);
        EXPECT_TRUE(array.AddUnique({5}) == true);

        array.Push({6});

        for (int32 i = 0; i < 6; ++i)
        {
            EXPECT_TRUE(*array[i].ValuePtr == i);
        }

        array.Insert(0, {-1});
        for (int32 i = 0; i < 6; ++i)
        {
            EXPECT_TRUE(*array[i].ValuePtr == i - 1);
        }

        NonTrivialArrayItem items2[] = {NonTrivialArrayItem(-3), NonTrivialArrayItem(-2)};
        array.Insert(0, items2, 2);
        for (int32 i = 0; i < 6; ++i)
        {
            EXPECT_TRUE(*array[i].ValuePtr == i - 3);
        }
    }

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
        EXPECT_TRUE(array.Empty());

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
        EXPECT_TRUE(array.Empty());

        array.Clear(1);
        EXPECT_TRUE(array.Empty() && array.Capacity() == 1);


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

        DynamicArray<int, FixedAllocator<5>> fixedArray;
        EXPECT_TRUE(fixedArray.Capacity() == 5);
        fixedArray = {5, 4, 3, 2, 1};

        array = fixedArray;
        EXPECT_TRUE(array[0] == 5);
        EXPECT_TRUE(array[4] == 1);

        DynamicArray<int> array2 = {5, 4, 3, 2, 1};
        EXPECT_TRUE(array == array2);

        DynamicArray<int, InlineAllocator<5>> inlineArray = {5, 4, 3, 2, 1};
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
            LOG_INFO("", "item of array is: {0}", value);
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
            LOG_INFO("", "item of array is: {0}", *iter);
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
            LOG_INFO("", "item of bit array is {0}", value);
        }

        for (auto iter = array.CreateValidIterator(); (bool)iter; ++iter)
        {
            LOG_INFO("", "item of bit array is {0}", *iter);
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
            LOG_INFO("", "destruct TestA");
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
            LOG_INFO("", "key:{0} value:{1}", pair.Key, pair.Value);
        }

        for (Map<int32, float>::ConstIterator iter = map.begin(); iter != map.end(); ++iter)
        {
            LOG_INFO("", "key:{0} value:{1}", iter->Key, iter->Value);
        }
    }
}