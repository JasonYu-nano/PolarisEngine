#include "gtest/gtest.h"
#include "foundation/array.hpp"
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

        NonTrivialArrayItem(NonTrivialArrayItem&& other) noexcept
        {
            ValuePtr = other.ValuePtr;
            other.ValuePtr = nullptr;
        }

        ~NonTrivialArrayItem()
        {
            --AllocCounter;
            delete ValuePtr;
            ValuePtr = nullptr;
        }

        NonTrivialArrayItem& operator=(const NonTrivialArrayItem& other)
        {
            *ValuePtr = *other.ValuePtr;
            return *this;
        }

        NonTrivialArrayItem& operator=(NonTrivialArrayItem&& other)
        {
            ValuePtr = other.ValuePtr;
            other.ValuePtr = nullptr;
            return *this;
        }

        bool operator== (const NonTrivialArrayItem& other) const
        {
            return *ValuePtr == *other.ValuePtr;
        }

        bool operator!= (const NonTrivialArrayItem& other) const
        {
            return *ValuePtr != *other.ValuePtr;
        }

        bool operator== (int32 val) const
        {
            return *ValuePtr == val;
        }

        bool operator!= (int32 val) const
        {
            return *ValuePtr != val;
        }

        uint32 GetHashCode() const
        {
            return *ValuePtr;
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

    TEST(ContainerTest, Array_CtorOtherAlloc)
    {
        Array<NonTrivialArrayItem, InlineAllocator<5>> array;
        array.Add({0});
        array.Add({1});
        array.Reserve(6);
        array.Add({2});
        array.RemoveAt(2);

        Array<NonTrivialArrayItem, InlineAllocator<5>> array2 = array;
        Array<NonTrivialArrayItem, InlineAllocator<5>> array3 = std::move(array2);
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
            EXPECT_TRUE(array[i] == i);
        }

        array.AddDefault();
        EXPECT_TRUE(array.Last() == 0);
        array.Last() = 4;

        EXPECT_TRUE(array.AddUnique(item) == false);
        EXPECT_TRUE(array.AddUnique({5}) == true);

        array.Push({6});

        for (int32 i = 0; i < 6; ++i)
        {
            EXPECT_TRUE(array[i] == i);
        }

        array.Insert(0, {-1});
        for (int32 i = 0; i < 6; ++i)
        {
            EXPECT_TRUE(array[i] == i - 1);
        }

        NonTrivialArrayItem items2[] = {NonTrivialArrayItem(-3), NonTrivialArrayItem(-2)};
        array.Insert(0, items2, 2);
        for (int32 i = 0; i < 6; ++i)
        {
            EXPECT_TRUE(array[i] == i - 3);
        }
    }

    TEST(ContainerTest, Array_Find)
    {
        Array<NonTrivialArrayItem> array = {NonTrivialArrayItem(0), NonTrivialArrayItem(1), NonTrivialArrayItem(2), NonTrivialArrayItem(0)};
        EXPECT_TRUE(array.Find(NonTrivialArrayItem(0)) == 0);
        EXPECT_TRUE(array.FindLast(NonTrivialArrayItem(0)) == 3);
    }

    TEST(ContainerTest, Array_Sort)
    {
        Array<int32> array = {5, 3, 1, 8, 9, 0};
        array.Sort();
        EXPECT_TRUE(array[0] == 0 && array[5] == 9);

        array.Sort(std::greater<>{});
        EXPECT_TRUE(array[0] == 9 && array[5] == 0);
    }

    TEST(ContainerTest, Array_Remove)
    {
        Array<NonTrivialArrayItem> array;
        NonTrivialArrayItem items[3] = {NonTrivialArrayItem(0), NonTrivialArrayItem(1), NonTrivialArrayItem(2)};
        array.Add(items, 3); // {0, 1, 2}

        array.RemoveAt(2, 1); // {0, 1}
        EXPECT_TRUE(array.Last() == 1);

        array.Add({2}); // {0, 1, 2}
        array.RemoveAt(0, 2); // {2}
        EXPECT_TRUE(array.Last() == 2);

        array.Add({3});
        array.Add({4}); // {2, 3, 4}
        array.Remove({3}); // {2, 4}
        EXPECT_TRUE(array[1] == 4);

        array.RemoveMatch([](const NonTrivialArrayItem& item){
            return item == 4;
        }); // {2}
        EXPECT_TRUE(array.Last() == 2);

        array.Add({3});
        array.Add({4}); // {2, 3, 4}
        array.RemoveAt(2);// {2, 3}
        EXPECT_TRUE(array.Last() == 3);

        array.Add({4});
        array.RemoveAtSwap(0); // {4, 3}
        EXPECT_TRUE(array.Last() == 3);

        array.Add({4}); // {4, 3, 4}
        auto elem = array.Pop();
        EXPECT_TRUE(elem == 4);

        array.Add({2}); // {4, 3, 2}
        array.RemoveSwap({2});
        EXPECT_TRUE(array.Last() == 3);
    }

    TEST(ContainerTest, Array_Resize)
    {
        Array<NonTrivialArrayItem> array;
        EXPECT_TRUE(array.Empty());

        array.Reserve(5);
        EXPECT_TRUE(array.Capacity() == 5);

        array.Shrink();
        EXPECT_TRUE(array.Capacity() == 0);

        array.Reserve(5);
        array.Reserve(3);
        EXPECT_TRUE(array.Capacity() == 5);

        array.Resize(3);
        EXPECT_TRUE(array.Capacity() == 5);
        EXPECT_TRUE(array.Size() == 3);

        auto item = NonTrivialArrayItem(1);
        array.Resize(2, item);
        EXPECT_TRUE(array.Top() == 0);

        array.Resize(3, item);
        EXPECT_TRUE(array.Top() == 1);

        array.Reserve(10);
        array.Shrink();
        EXPECT_TRUE(array.Capacity() == 3);
    }

    TEST(ContainerTest, Array_Iterator)
    {
        Array<int32> array = {0, 1, 2, 3, 4, 5};

        int32 idx = 0;
        for (const int32& item : array)
        {
            EXPECT_TRUE(item == idx);
            ++idx;
        }

        for (auto it = array.rbegin(); (bool)it; --it)
        {
            EXPECT_TRUE(it.GetIndex() == *it);
        }

        for (auto it = array.rbegin(); it != array.rend(); --it)
        {
            EXPECT_TRUE(it.GetIndex() == *it);
        }

        for (auto it = array.cbegin(); it != array.cend(); ++it)
        {
            EXPECT_TRUE(it.GetIndex() == *it);
        }

        for (auto it = array.crbegin(); it != array.crend(); --it)
        {
            EXPECT_TRUE(it.GetIndex() == *it);
        }

        for (auto it = array.begin(); it != array.end(); ++it)
        {
            if (*it == 2 || *it == 0)
            {
                it.RemoveSelf();
            }
        }
        EXPECT_TRUE(array[0] == 1 && array[3] == 5);
    }

    TEST(ContainerTest, BitArray_Ctor)
    {
        BitArray array(10);
        BitArray array2(true, 10);
        BitArray array3(false, 10);

        bool list[5] = { true, false, true, false, true};
        BitArray array4(list, 5);
        EXPECT_TRUE(array4.Size() == 5);
        EXPECT_TRUE(array4[0] == true && array4[1] == false && array4[2] == true && array4[3] == false && array4[4] == true);

        BitArray array5 = {false, true, false, true, false};
        EXPECT_TRUE(array5.Size() == 5);
        EXPECT_TRUE(array5[0] == false && array5[1] == true && array5[2] == false && array5[3] == true && array5[4] == false);

        array4.Resize(10, true);
        array5.Resize(10, false);

        BitArray array6(array4);
        BitArray array7(std::move(array5));
        EXPECT_TRUE(array5.Size() == 0 && array5.Capacity() == 0);
        EXPECT_TRUE(array6[0] == true && array6[1] == false && array6[2] == true && array6[3] == false && array6[4] == true);
        EXPECT_TRUE(array7[0] == false && array7[1] == true && array7[2] == false && array7[3] == true && array7[4] == false);

        EXPECT_TRUE(array6 == array4);
    }

    TEST(ContainerTest, BitArray_Add)
    {
        BitArray array = {true, false, true, false};
        array.Insert(1, true);
        EXPECT_TRUE(array[1] == true);

        array.Insert(5, true);
        EXPECT_TRUE(array[5] == true);
    }

    TEST(ContainerTest, BitArray_Find)
    {
        BitArray array = {true, false, true, false};
        EXPECT_TRUE(array.Find(true) == 0);
        EXPECT_TRUE(array.Find(false) == 1);
        EXPECT_TRUE(array.FindLast(true) == 2);
        EXPECT_TRUE(array.FindLast(false) == 3);

        BitArray array2 = {false, false, false, false};
        EXPECT_TRUE(array2.FindLast(true) == INDEX_NONE);
        EXPECT_TRUE(array2.Find(true) == INDEX_NONE);

        array.RemoveAt(1, 2);
        EXPECT_TRUE(array.Size() == 2);
        EXPECT_TRUE(array[1] == false);
    }

    TEST(ContainerTest, BitArray_Iterator)
    {
        BitArray array = {true, false, true, false};
        bool compareVal = true;
        for (auto val : array)
        {
            EXPECT_TRUE(val == compareVal);
            compareVal = !compareVal;
        }

        for (auto it = array.CreateValidIterator(); (bool)it; ++it)
        {
            EXPECT_TRUE(*it);
            EXPECT_TRUE(it.GetIndex() % 2 == 0);
        }
    }

    TEST(ContainerTest, SparseArray_Ctor)
    {
        SparseArray<NonTrivialArrayItem> array(10);
        EXPECT_TRUE(array.Capacity() == 10);

        NonTrivialArrayItem items[3] = {NonTrivialArrayItem(0), NonTrivialArrayItem(1), NonTrivialArrayItem(2)};
        SparseArray<NonTrivialArrayItem> array1(items, 3);

        EXPECT_TRUE(array1[0] == 0 && array1[1] == 1 && array1[2] == 2);

        SparseArray<NonTrivialArrayItem> array2(array1);
        EXPECT_TRUE(array2[0] == 0 && array2[1] == 1 && array2[2] == 2);

        SparseArray<NonTrivialArrayItem> array3(std::move(array1));
        EXPECT_TRUE(array3[0] == 0 && array3[1] == 1 && array3[2] == 2);
    }

    TEST(ContainerTest, SparseArray_Add)
    {
        SparseArray<NonTrivialArrayItem> array(10);
        array.Add({0});
        array.Add({1});
        array.Add({2});
        EXPECT_TRUE(array[0] == 0 && array[1] == 1 && array[2] == 2);

        array.RemoveAt(1);
        array.Add({3});
        EXPECT_TRUE(array[1] == 3);

        array.RemoveAt(2);
        array.Insert(3, {4});
        EXPECT_TRUE(array[3] == 4);

        EXPECT_FALSE(array.HasValue(2));

        array.Shrink();
        array.Compact();
        EXPECT_TRUE(array[2] == 4);
    }

    TEST(ContainerTest, SparseArray_Iter)
    {
        SparseArray<NonTrivialArrayItem> array(10);
        array.Add({0});
        array.Add({1});
        array.Add({2});
        array.RemoveAt(1);
        array.RemoveAt(2);
        array.Insert(3, {4});

        int32 count = 0;
        for (auto& val : array)
        {
            ++count;
        }
        EXPECT_TRUE(count = 2);
    }

    TEST(ContainerTest, Set_Ctor)
    {
        Set<NonTrivialArrayItem> set(10);

        Set<NonTrivialArrayItem> set1 = { NonTrivialArrayItem(0), NonTrivialArrayItem(1), NonTrivialArrayItem(2) };

        Set<NonTrivialArrayItem> set2(set1);

        Set<NonTrivialArrayItem> set3(set);

        Set<NonTrivialArrayItem> set4(std::move(set));
    }

    TEST(ContainerTest, Set_Modify)
    {
        Set<NonTrivialArrayItem> set = { NonTrivialArrayItem(0), NonTrivialArrayItem(1), NonTrivialArrayItem(2) };
        set.Add(NonTrivialArrayItem{0});
        EXPECT_TRUE(set.Contains(NonTrivialArrayItem{0}));

        NonTrivialArrayItem* item = set.Find(NonTrivialArrayItem{1});
        EXPECT_TRUE(item != nullptr);

        set.Remove(NonTrivialArrayItem{1});

        item = set.Find(NonTrivialArrayItem{1});
        EXPECT_TRUE(item == nullptr);
    }

    TEST(ContainerTest, Set_Iterator)
    {
        Set<NonTrivialArrayItem> set = { NonTrivialArrayItem(0), NonTrivialArrayItem(1), NonTrivialArrayItem(2) };

        int32 count = 0;
        for (auto&& item : set)
        {
           ++count;
        }
        EXPECT_TRUE(count == 3);
    }

    TEST(ContainerTest, Map_Ctor)
    {
        Map<int32, NonTrivialArrayItem> map(10);
        Map<int32, NonTrivialArrayItem> map1 = {{0, NonTrivialArrayItem(0)}, {1, NonTrivialArrayItem(1)}, {2, NonTrivialArrayItem(2)}};
        EXPECT_TRUE(map1.FindRef(0) == 0);
        EXPECT_TRUE(map1.FindRef(1) == 1);
        EXPECT_TRUE(map1.FindRef(2) == 2);

        Map<int32, NonTrivialArrayItem> map2(map1);
        EXPECT_TRUE(map2.FindRef(0) == 0);
        EXPECT_TRUE(map2.FindRef(1) == 1);
        EXPECT_TRUE(map2.FindRef(2) == 2);

        Map<int32, NonTrivialArrayItem> map3(std::move(map1));
        EXPECT_TRUE(map3.FindRef(0) == 0);
        EXPECT_TRUE(map3.FindRef(1) == 1);
        EXPECT_TRUE(map3.FindRef(2) == 2);
    }

    TEST(ContainerTest, Map_Modify)
    {
        Map<int32, NonTrivialArrayItem> map(10);
        map.Add(0, NonTrivialArrayItem(0));
        EXPECT_TRUE(map.FindRef(0) == 0);

        NonTrivialArrayItem item(1);
        map.Add(1, item);
        EXPECT_TRUE(map.FindRef(1) == 1);

        int32 key = 1;
        map.Add(key, NonTrivialArrayItem(2));
        EXPECT_TRUE(map.FindRef(1) == 2);

        map.FindOrAdd(2, NonTrivialArrayItem(2));
        EXPECT_TRUE(map.FindRef(2) == 2);

        map.FindOrAdd(2, NonTrivialArrayItem(3));
        EXPECT_TRUE(map.FindRef(2) == 2);

        EXPECT_TRUE(map.Contains(2));

        map.Remove(1);
        EXPECT_FALSE(map.Contains(1));

        EXPECT_TRUE(map.Size() == 2);
    }

    TEST(ContainerTest, Map_Iterator)
    {
        Map<int32, NonTrivialArrayItem> map = {{0, NonTrivialArrayItem(0)}, {1, NonTrivialArrayItem(1)}, {2, NonTrivialArrayItem(2)}};

        for (auto&& pair : map)
        {
            EXPECT_TRUE(pair.Key == pair.Value);
        }

        for (auto it = map.begin(); (bool)it; ++it)
        {
            EXPECT_TRUE(it->Key == it->Value);
        }
    }
}