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
    }

    TEST(TupleTest, Get)
    {
        std::string name = "string";
        int32 level = 5;

        auto t0 = MakeTuple(name, level, 10.0f);

        EXPECT(t0.Get<std::string>() == "string");
    }

    TEST(TupleTest, Compare)
    {
        auto t0 = MakeTuple(20, 19, 18.5f);
        auto t1 = MakeTuple(19, 18, 18.0f);
        auto t2 = MakeTuple(19, 19, 18.0f);

        EXPECT(t0 > t1);
        EXPECT(t1 < t0);
        EXPECT_FALSE(t0 > t2);
        EXPECT_FALSE(t0 < t2);

        const auto t3 = MakeTuple(std::string{"string"}, 7, 10.0f);
        EXPECT(t3 == MakeTuple(std::string{"string"}, 7, 10.0f));
    }
}