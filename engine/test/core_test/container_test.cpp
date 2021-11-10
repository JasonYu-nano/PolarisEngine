#include "gtest/gtest.h"
#include "foundation/list.hpp"
#include "memory/memory.hpp"

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

    TEST(ListTest, Construct)
    {
        List<int> list;
        list.Add(1);
        EXPECT_TRUE(list[0] == 1);
        list[0] = 10;
        EXPECT_TRUE(list[0] == 10);

        list.Insert(0, 9);
        list.Insert(2, 11);
        EXPECT_TRUE(list[0] == 9 && list[1] == 10 && list[2] == 11);

        EXPECT_TRUE(list.GetCount() == 3);
        list.Clear(1);
        EXPECT_TRUE(list.GetCount() == 0 && list.GetCapacity() == 1);


        List<ListTestStruct> listOfStruct(8);
        listOfStruct.Add(ListTestStruct());
        listOfStruct.Add(ListTestStruct());
        listOfStruct.Clear();

        List<int> listOfCopy = { 1, 2, 3, 4, 0 };
        EXPECT_TRUE(listOfCopy[0] == 1 && listOfCopy[4] == 0);
        List<int> listOfCopy2(4);
        listOfCopy2 = MoveTemp(listOfCopy);
        EXPECT_TRUE(listOfCopy2[0] == 1 && listOfCopy2[4] == 0);
    }
}