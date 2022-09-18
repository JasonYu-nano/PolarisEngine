#include "gtest/gtest.h"
#include "math/vector.hpp"

namespace Engine
{
    TEST(Vector3Test, Base)
    {
        EXPECT_TRUE(Vector3f::Zero.IsZero());
        EXPECT_TRUE(Vector3f::XAxis.IsUnit());
        EXPECT_TRUE(Vector3f::YAxis.IsUnit());
        EXPECT_TRUE(Vector3f::ZAxis.IsUnit());

        Vector3f vec(3.0f, 4, 5);
        vec.Normalize();
        EXPECT_TRUE(vec.IsNormalized());

        EXPECT_TRUE(vec * 2.5f / 2.5f == vec);

        vec ^ Vector3f(5.2);
    }
}