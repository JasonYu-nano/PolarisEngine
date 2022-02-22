#include "gtest/gtest.h"
#include "core_minimal_public.hpp"
#include "reflection/variant.hpp"
#include "reflection/field.hpp"

namespace Engine
{
    class TestClass
    {
    public:
        int32 A{ 1 };
    };


    TEST(RelflectionTest, Variant)
    {
        int numeric = 1;
        Variant var(numeric);
        int& val = var.GetValue<int>();
        EXPECT_TRUE(val == numeric);
        val = 2;

        EXPECT_TRUE(val == numeric);
        int* ptr = var.GetPtr<int>();
        *ptr = 3;
        EXPECT_TRUE(numeric == 3);

        Variant var2(numeric, VARIANT_COPY);
        int& val2 = var2.GetValue<int>();
        val2 = 4;
        EXPECT_TRUE(numeric == 3);

        Variant var3(0);
        var3.GetValue<int>() = 1;
        EXPECT_TRUE(var3.GetValue<int>() == 1);

        Variant var4(var);
        EXPECT_TRUE(var4.GetPtr<int>() == var.GetPtr<int>());

        Variant var5(MoveTemp(var4));
        EXPECT_FALSE(var4.IsValid());
    }

    TEST(RelflectionTest, MetaProperty)
    {
        TestClass inst;

        MetaProperty prop(new PropertyInst(&TestClass::A));
        Variant var = prop.GetValue(inst);
        EXPECT_TRUE(var.GetValue<int>() == inst.A);

        prop.SetValue(inst, 2);
        EXPECT_TRUE(inst.A == 2);
    }
}