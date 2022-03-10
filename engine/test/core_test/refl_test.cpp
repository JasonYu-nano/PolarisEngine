#include "gtest/gtest.h"
#include "core_minimal_public.hpp"
#include "reflection/variant.hpp"
#include "reflection/field.hpp"
#include "reflection/method.hpp"

namespace Engine
{
    class TestClass
    {
    public:
        static String GetName()
        {
            return _T("TestClass");
        }

        int32 GetA() const
        {
            return A;
        }

        void SetA(int32 newA)
        {
            A = newA;
        }

        int32& GetARef()
        {
            return A;
        }

        int32 A{ 1 };
    };


    TEST(RelflectionTest, Variant)
    {
        int32 numeric = 1;
        Variant var(numeric);
        int32& val = var.GetValue<int32>();
        EXPECT_TRUE(val == numeric);
        val = 2;

        EXPECT_TRUE(val == numeric);
        int32* ptr = var.GetPtr<int32>();
        *ptr = 3;
        EXPECT_TRUE(numeric == 3);

        Variant var2(numeric, VARIANT_COPY);
        int32& val2 = var2.GetValue<int32>();
        val2 = 4;
        EXPECT_TRUE(numeric == 3);

        Variant var3(0);
        var3.GetValue<int32>() = 1;
        EXPECT_TRUE(var3.GetValue<int32>() == 1);

        Variant var4(var);
        EXPECT_TRUE(var4.GetPtr<int32>() == var.GetPtr<int32>());

        Variant var5(MoveTemp(var4));
        EXPECT_FALSE(var4.IsValid());

        Variant var6(nullptr);
        EXPECT_TRUE(var6.GetValue<int32*>() == nullptr);
    }

    TEST(RelflectionTest, Field)
    {
        TestClass inst;

        Field field(new FieldEntity(&TestClass::A));
        Variant var = field.GetValue(inst);
        EXPECT_TRUE(var.GetValue<int32>() == inst.A);

        field.SetValue(inst, 2);
        EXPECT_TRUE(inst.A == 2);
    }

    TEST(RelflectionTest, Method)
    {
        TestClass inst;

        Method method(new MethodInvoker(&TestClass::GetA));
        Variant ret = method.Invoke({inst});
        EXPECT_TRUE(ret.GetValue<int32>() == 1);

        Method method2(new MethodInvoker(&TestClass::GetARef));
        Variant ret2 = method2.Invoke({inst});
        EXPECT_TRUE(ret2.GetValue<int32>() == 1);

        int32& val = ret2.GetValue<int32>();
        val = 2;
        EXPECT_TRUE(inst.A == 2);
        EXPECT_TRUE(ret.GetValue<int32>() == 1);

        String str = _T("abc");
        ParamPack args{2, str};

        EXPECT_TRUE(args[0].GetValue<int32>() == 2 && args[1].GetValue<String>() == _T("abc"));

        Method method3(new MethodInvoker(&TestClass::SetA));
        Variant ret3 = method3.Invoke({inst, 2});
        EXPECT_TRUE(inst.A == 2);
    }

    TEST(RelflectionTest, StaticMethod)
    {
        TestClass inst;

        Method method(new StaticMethodInvoker(&TestClass::GetName));
        Variant ret = method.Invoke({});
        EXPECT_TRUE(ret.GetValue<String>() == _T("TestClass"));
    }
}