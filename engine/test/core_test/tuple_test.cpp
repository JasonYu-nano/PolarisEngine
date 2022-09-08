#include "gtest/gtest.h"
#include <tuple>
#include "foundation/tuple.hpp"
#include "log/logger.hpp"
#include "foundation/delegate.hpp"


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
        EXPECT_TRUE(name == "nano");
        EXPECT_TRUE(level == 7);

        EXPECT_TRUE(t1.Get<0>() == "nano");
        EXPECT_TRUE(t1.Get<1>() == 7);
        EXPECT_TRUE(std::is_reference_v<decltype(get<2>(t0))>);
        EXPECT_TRUE(std::is_reference_v<decltype(t1.Get<2>())>);

        EXPECT_TRUE(t2.Get<0>() == "name");
        EXPECT_TRUE(t2.Get<1>() == 5);
    }

    TEST(TupleTest, Get)
    {
        std::string name = "string";
        int32 level = 5;

        auto t0 = MakeTuple(name, level, 10.0f);

        EXPECT_TRUE(t0.Get<std::string>() == "string");
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

        static void OnMultiDelegateBroadcastStatic(const UString& name)
        {
            LastDelegateName = name;
        }

        UString GetName() const { return LastDelegateName; }

        void OnMultiDelegateBroadcastMember(const UString& name, int32 additionNumber)
        {
            ReceiveBroadcastCount += additionNumber;
        }

        static UString LastDelegateName;

        int32 ReceiveBroadcastCount = 0;
    };

    UString DelegateTestClass::LastDelegateName = "";

    TEST(DelegateTest, All)
    {
        DECLARE_DELEGATE_ONE_PARAM(DelegateType, const UString&);
        DECLARE_DELEGATE_RET(GetNameDelegate, UString);

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

    TEST(MultiDelegateTest, All)
    {
        DECLARE_MULTI_DELEGATE_ONE_PARAM(Notifier, const UString&);

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

            void OnMultiDelegateBroadcastMember(const UString& name)
            {
                ReceiveBroadcastCount += 1;
            }

            void OnMultiDelegateBroadcastMemberRemoveSelf(const UString& name)
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