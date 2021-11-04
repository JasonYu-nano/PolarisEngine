#include "gtest/gtest.h"
#include "foundation/list.hpp"

namespace Engine
{
    struct ListTestStruct
    {
        bool X{ false };
        int32 Y{ 0 };
        int32* Z{ nullptr };
    };

    TEST(ListTest, Construct)
    {
        List<int> list;
        list.Add(1);
    }
}