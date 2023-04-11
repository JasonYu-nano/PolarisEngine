#include "gtest/gtest.h"
#include "global.hpp"
#include "foundation/string.hpp"
#include "mos_test_header.hpp"

using namespace Engine;

namespace Test
{
    struct Pod
    {
        int32 Int = 0;
        float* PFloat = nullptr;
    };

    class NonStandardLayoutBase
    {
    public:

        virtual int32 GetInt() = 0;

        int32 Int = 0;
        float Float = 0.0f;

        constexpr static int32 Counter = 0;
    };

    class NonStandardLayoutChild : public NonStandardLayoutBase
    {
    public:

        int32 GetInt() override { return Int; }

    private:
        void* Data;

    public:
        String Name;
    };
}

TEST(MosTest, TestOffset)
{
    EXPECT_TRUE(OFFSET_OF(::Test::Pod, Int) == 0);
    EXPECT_TRUE(OFFSET_OF(::Test::Pod, PFloat) == 8);

    EXPECT_TRUE(OFFSET_OF(::Test::NonStandardLayoutBase, Int) == 8);
    EXPECT_TRUE(OFFSET_OF(::Test::NonStandardLayoutChild, Name) == 24);
}

TEST(MosTest, TestMetaObjectTraits)
{
    EXPECT_FALSE(HasMetaClassV<MOSTestClassBase>);
    EXPECT_TRUE(HasMetaStructV<MOSTestStruct>);
}