#include "gtest/gtest.h"
#include "Taskflow.hpp"

namespace Engine
{
    TEST(TaskTest, Base)
    {
        Taskflow taskflow;
        auto lambda = [](){};
        auto task = taskflow.Add(lambda);
    }
}