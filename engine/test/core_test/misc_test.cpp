#include "gtest/gtest.h"
#include "core_minimal_public.hpp"
#include "foundation/char_utils.hpp"
#include "math/city_hash.hpp"
#include "foundation/fixed_string.hpp"

namespace Engine
{
    TEST(CharTest, Base)
    {
        ansi c = 'C';
        c = CharUtils::ToLower(c);
        EXPECT_TRUE(c == 'c');

        c = CharUtils::ToUpper(c);
        EXPECT_TRUE(c == 'C');
    }

    TEST(FixedString, Base)
    {
        FixedString name(_T("Hello_World"));
        EXPECT_TRUE(name.ToString() == _T("Hello_World"));

        FixedString name2(_T("Hello_World_12"));
        EXPECT_TRUE(name2.ToString() == _T("Hello_World_12") && name2.GetNumber() == 12);

        FixedString name3(_T("Hello_World_012"));
        EXPECT_TRUE(name3.GetNumber() == 0);

        FixedString name4(_T("hello_World_12"));
        EXPECT_TRUE(name2 == name4);
        EXPECT_TRUE(name4.ToString() == _T("Hello_World_12"));
    }
}
