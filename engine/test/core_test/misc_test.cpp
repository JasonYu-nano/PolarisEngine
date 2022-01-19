#include "gtest/gtest.h"
#include "core_minimal_public.hpp"
#include "foundation/char_utils.hpp"
#include "math/city_hash.hpp"
#include "foundation/fixed_string.hpp"

namespace Engine
{
    TEST(CharTest, Base)
    {
        schar c = 'C';
        c = CharUtils::ToLower(c);
        EXPECT_TRUE(c == 'c');

        c = CharUtils::ToUpper(c);
        EXPECT_TRUE(c == 'C');
    }

    TEST(FixedString, Base)
    {
        FixedString name(TC("Hello_World"));
        EXPECT_TRUE(name.ToString() == TC("Hello_World"));

        FixedString name2(TC("Hello_World_12"));
        EXPECT_TRUE(name2.ToString() == TC("Hello_World_12") && name2.GetNumber() == 12);

        FixedString name3(TC("Hello_World_012"));
        EXPECT_TRUE(name3.GetNumber() == 0);

        FixedString name4(TC("hello_World_12"));
        EXPECT_TRUE(name2 == name4);
        EXPECT_TRUE(name4.ToString() == TC("Hello_World_12"));
    }
}
