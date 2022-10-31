#include "gtest/gtest.h"
#include "taskflow.hpp"

namespace Engine
{
    TEST(TaskTest, Base)
    {
        Taskflow taskflow;
        auto lambda = [](){};
        auto task = taskflow.Add(lambda);
    }
}