#include "gtest/gtest.h"
#include "object_module.hpp"
#include "module/module_manager.hpp"

using namespace Engine;

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    ModuleManager::Load<ObjectModule>("Object");

    auto ret = RUN_ALL_TESTS();

    return ret;
}