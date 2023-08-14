#include "gtest/gtest.h"
#include <tuple>
#include "foundation/tuple.hpp"
#include "log/logger.hpp"
#include "foundation/delegate.hpp"


namespace Engine
{
    TEST(TupleTest, Ctor)
    {
        int x = 42;
        float y = 3.14f;
        float z = 1.001f;

        std::tuple<int&, float&, int> t0(x, y, static_cast<int>(z));

        std::get<0>(t0) = 100;
        std::get<1>(t0) = 2.718f;

        EXPECT_TRUE(x == 100 && y == 2.718f);
        EXPECT_TRUE(std::is_reference_v<decltype(std::get<1>(t0))>);
        EXPECT_TRUE(std::is_rvalue_reference_v<decltype(std::get<2>(std::move(t0)))>);

        Tuple<int&, float&, int> t1(x, y, static_cast<int>(z));

        t1.Get<0>() = 200;
        t1.Get<1>() = 5.718f;

        EXPECT_TRUE(x == 200 && y == 5.718f);
        EXPECT_TRUE(std::is_reference_v<decltype(t1.Get<1>())>);
        EXPECT_TRUE(std::is_rvalue_reference_v<decltype(std::move(t1).Get<1>())>);
    }

    TEST(TupleTest, Compare)
    {
        auto t0 = MakeTuple(20, 19, 18.5f);
        auto t1 = MakeTuple(19, 18, 18.0f);
        auto t2 = MakeTuple(19, 19, 18.0f);

        EXPECT_TRUE(t0 > t1);
        EXPECT_TRUE(t1 < t0);
        EXPECT_FALSE(t0 > t2);
        EXPECT_FALSE(t0 < t2);

        const auto t3 = MakeTuple(std::string{"string"}, 7, 10.0f);
        EXPECT(t3 == MakeTuple(std::string{"string"}, 7, 10.0f));
    }

    struct DelegateTestClass
    {
        static int32 Add(int32 a, int32 b)
        {
            return a + b;
        }

        static void OnMultiDelegateBroadcastStatic(const String& name)
        {
            LastDelegateName = name;
        }

        String GetName() const { return LastDelegateName; }

        void OnMultiDelegateBroadcastMember(const String& name, int32 additionNumber)
        {
            ReceiveBroadcastCount += additionNumber;
        }

        static String LastDelegateName;

        int32 ReceiveBroadcastCount = 0;
    };

    String DelegateTestClass::LastDelegateName = "";

    TEST(DelegateTest, Delegate)
    {
        DECLARE_DELEGATE_ONE_PARAM(DelegateType, const String&);
        DECLARE_DELEGATE_RET(GetNameDelegate, String);

        DelegateType delegate = DelegateType::CreateStatic(&DelegateTestClass::OnMultiDelegateBroadcastStatic);
        delegate.Execute("SingleDelegate");
        EXPECT_TRUE(DelegateTestClass::LastDelegateName == "SingleDelegate");

        const DelegateTestClass inst;
        GetNameDelegate delegate1 = GetNameDelegate::CreateRaw(&inst, &DelegateTestClass::GetName);
        EXPECT_TRUE(delegate1.Execute() == "SingleDelegate");

        SharedPtr<DelegateTestClass> inst2 = MakeShared<DelegateTestClass>();
        delegate1.Unbind();
        delegate1.BindSP(inst2, &DelegateTestClass::GetName);
        EXPECT_TRUE(delegate1.Execute() == "SingleDelegate");
    }

    TEST(DelegateTest, MultiDelegate)
    {
        DECLARE_MULTI_DELEGATE_ONE_PARAM(Notifier, const String&);

        SharedPtr<DelegateTestClass> testClass = MakeShared<DelegateTestClass>();

        Notifier multiDelegate;
        multiDelegate.AddStatic(&DelegateTestClass::OnMultiDelegateBroadcastStatic);
        DelegateHandle handle0 = multiDelegate.AddRaw(testClass.get(), &DelegateTestClass::OnMultiDelegateBroadcastMember, 1);
        DelegateHandle handle1 = multiDelegate.AddSP(testClass, &DelegateTestClass::OnMultiDelegateBroadcastMember, 1);

        multiDelegate.BroadcastIfBound("MultiDelegate");

        EXPECT_TRUE(DelegateTestClass::LastDelegateName == "MultiDelegate");
        EXPECT_TRUE(testClass->ReceiveBroadcastCount == 2);

        multiDelegate.Remove(handle1);
        multiDelegate.BroadcastIfBound("MultiDelegate");
        EXPECT_TRUE(testClass->ReceiveBroadcastCount == 3);

        struct RemoveSelfDelegateTestClass
        {
            RemoveSelfDelegateTestClass(Notifier& notifier) : Delegate(notifier)
            {
                notifier.AddRaw(this, &RemoveSelfDelegateTestClass::OnMultiDelegateBroadcastMember);
                RemoveHandle = notifier.AddRaw(this, &RemoveSelfDelegateTestClass::OnMultiDelegateBroadcastMemberRemoveSelf);
            }

            void OnMultiDelegateBroadcastMember(const String& name)
            {
                ReceiveBroadcastCount += 1;
            }

            void OnMultiDelegateBroadcastMemberRemoveSelf(const String& name)
            {
                ReceiveBroadcastCount += 1;
                Delegate.Remove(RemoveHandle);
            }


            Notifier& Delegate;
            DelegateHandle RemoveHandle;
            int32 ReceiveBroadcastCount = 0;
        };

        RemoveSelfDelegateTestClass removeSelfTest(multiDelegate);

        multiDelegate.BroadcastIfBound("MultiDelegate");
        EXPECT_TRUE(removeSelfTest.ReceiveBroadcastCount == 2);

        multiDelegate.RemoveAll(testClass.get());
        EXPECT_TRUE(multiDelegate.GetBoundNum() == 2);

        struct EventTestClass
        {
            DECLARE_EVENT(GTestEvent, EventTestClass);

            EventTestClass()
            {
                Event.Broadcast();
            }

            GTestEvent Event;
        };
    }
}