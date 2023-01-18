#include "gtest/gtest.h"
#include "math/vector.hpp"
#include "math/rotator.hpp"
#include "math/quaternion.hpp"
#include "math/matrix.hpp"
#include "log/logger.hpp"

namespace Engine
{
    TEST(MathTest, GenericMath)
    {
        EXPECT_TRUE(Math::Equals(Math::Sin(Math::ToRadians(30)), Math::Cos(Math::ToRadians(60)), KINDA_SMALL_FLOAT));
        EXPECT_TRUE(Math::Equals(Math::Sin(Math::ToRadians(30)), Math::Sin(Math::ToRadians(-570)), KINDA_SMALL_FLOAT));
        EXPECT_TRUE(Math::FloorLogTwo((uint32)33) == 5);
        EXPECT_TRUE(Math::FloorLogTwo((uint8)31) == 4);
        EXPECT_TRUE(Math::CeilLogTwo((uint32)33) == 6);
        EXPECT_TRUE(Math::CeilLogTwo((uint8)31) == 5);
    }

    TEST(Vector3Test, Base)
    {
        EXPECT_TRUE(Vector3f::Zero.IsZero());

        Vector3f vec(3.0f, 4, 5);
        vec.Normalize();
        EXPECT_TRUE(vec.IsNormalized());

        EXPECT_TRUE(vec * 2.5f / 2.5f == vec);

        vec ^ Vector3f(5.2f);
    }

    TEST(RotatorTest, Base)
    {
        Rotator rotation(10.0f, 60.0f, 10.0f);
        Quat quat = rotation.ToQuaternion();
        quat.Normalize();
        quat = Quat::Identity;
    }

    TEST(MatrixTest, Base)
    {
        Matrix M;
        Matrix N;
        for (int32 i = 0; i < 4; ++i)
        {
            for (int32 j = 0; j < 4; ++j)
            {
                M.M[i][j] = i + j;
                N.M[i][j] = i - j;
            }
        }

        M = M * N;
    }
}