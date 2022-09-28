#include "gtest/gtest.h"
#include "math/vector.hpp"
#include "math/rotator.hpp"
#include "math/quaternion.hpp"

namespace Engine
{
    TEST(MathTest, Base)
    {
        EXPECT_TRUE(Math::Equals(Math::Sin(Math::ToRadians(30)), Math::Cos(Math::ToRadians(60)), KINDA_SMALL_FLOAT));
        EXPECT_TRUE(Math::Equals(Math::Sin(Math::ToRadians(30)), Math::Sin(Math::ToRadians(-570)), KINDA_SMALL_FLOAT));
    }

    TEST(Vector3Test, Base)
    {
        EXPECT_TRUE(Vector3f::Zero.IsZero());

        Vector3f vec(3.0f, 4, 5);
        vec.Normalize();
        EXPECT_TRUE(vec.IsNormalized());

        EXPECT_TRUE(vec * 2.5f / 2.5f == vec);

        vec ^ Vector3f(5.2);
    }

    TEST(RotatorTest, Base)
    {
        Rotator rotation(10.0f, 60.0f, 10.0f);
        Quat quat = rotation.ToQuaternion();
        quat = Quat::Identity;
    }
}