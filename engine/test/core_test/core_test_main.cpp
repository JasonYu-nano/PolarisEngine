#include "gtest/gtest.h"
#include "mos/meta_register.hpp"

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    Engine::MetaObjectRegistry::Initialize();

    auto ret = RUN_ALL_TESTS();

    Engine::MetaObjectRegistry::Finalize();

    return ret;
}