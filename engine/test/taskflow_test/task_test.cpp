#include "gtest/gtest.h"
#include "task_executor.hpp"

namespace Engine
{
    TEST(TaskTest, Base)
    {
        Taskflow taskflow;
        auto lambda = [](){};
        auto& task = taskflow.Add<decltype(lambda)>(lambda);
        GExecutor.Execute(taskflow);
    }
}