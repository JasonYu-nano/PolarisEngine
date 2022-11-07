#include "gtest/gtest.h"
#include "../../source/taskflow/src/task_executor.hpp"

namespace Engine
{
    TEST(TaskTest, Creation)
    {
        class ThreadBlockingTask
        {
        public:
            ThreadBlockingTask(const std::string& name) : TaskName(name) {}

            void operator() ()
            {
                std::cout<<TaskName<<" runs on thread: "<<std::this_thread::get_id()<<std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                std::cout<<TaskName<<" completed"<<std::endl;
            }

        private:
            std::string TaskName;
        };

        Taskflow taskflow;
        auto& taskA = taskflow.Add<ThreadBlockingTask>("Task A");

        auto& taskB = taskflow.Add<ThreadBlockingTask>("Task B");

        auto& taskC = taskflow.Add<ThreadBlockingTask>("Task C");

        taskA-->taskC;

        taskflow.Execute();
        taskflow.Wait();
        std::cout<<"Taskflow completed"<<std::endl;

    }

    TEST(TaskTest, Execution)
    {
        Taskflow taskflow;
        auto& taskA = taskflow.Add([](){
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::cout<<"Task A running"<<std::endl;
        });

        auto& taskB = taskflow.Add([](){
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::cout<<"Task B running"<<std::endl;
        });

        auto& taskC = taskflow.Add([](){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout<<"Task C running"<<std::endl;
        });

        auto& taskD = taskflow.Add([](){
            std::cout<<"Task D running"<<std::endl;
        });

        taskA-->taskB-->taskD;
        taskA-->taskC-->taskD;

        taskflow.Execute();
        taskflow.Wait();
        std::cout<<"Taskflow completed"<<std::endl;

    }
}