#include "gtest/gtest.h"
#include "../../source/taskflow/src/task_executor.hpp"

namespace Engine
{
    TEST(TaskTest, Base)
    {
        Taskflow taskflow;
        auto& task = taskflow.Add([](){
            std::cout<<"11111"<<std::endl;
        });
        taskflow.Execute();
    }
}