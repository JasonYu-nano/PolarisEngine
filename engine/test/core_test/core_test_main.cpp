#include "gtest/gtest.h"
#include "foundation/encoding.hpp"

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    Engine::SetupLocale();

    auto ret = RUN_ALL_TESTS();

    Engine::ShutdownLocale();

    return ret;
}