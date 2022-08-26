#include "gtest/gtest.h"
#include <tuple>
#include "foundation/tuple.hpp"
#include "log/logger.hpp"


namespace Engine
{
    TEST(TupleTest, Ctor)
    {
        std::string name = "name";
        int32 level = 5;

        std::tuple<std::string&, int32&, int32> t0 = {name, level, 10};
        Tuple<std::string&, int32&, int32> t1 {name, level, 10};
        auto t2 = MakeTuple(name, level, 10);

        std::get<0>(t0) = "nano";
        std::get<1>(t0) = 7;
        EXPECT(name == "nano");
        EXPECT(level == 7);

        EXPECT(t1.Get<0>() == "nano");
        EXPECT(t1.Get<1>() == 7);
        EXPECT(std::is_reference_v<decltype(get<2>(t0))>);
        EXPECT(std::is_reference_v<decltype(t1.Get<2>())>);

        EXPECT(t2.Get<0>() == "name");
        EXPECT(t2.Get<1>() == 5);

        const auto t3 = MakeTuple(name, level, 10);
        EXPECT(t3 == MakeTuple(std::string{name}, 7, 10));
    }

    TEST(TupleTest, Get)
    {
        std::string name = "string";
        int32 level = 5;

        auto t0 = MakeTuple(name, level, 10.0f);

        EXPECT(t0.Get<std::string>() == "string");
    }
}