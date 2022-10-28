#pragma once

#include "math/vector.hpp"

namespace Engine
{
    struct alignas(16) CORE_API Matrix
    {
        Matrix() = default;

        Matrix(const Vector4f& x, const Vector4f& y, const Vector4f& z, const Vector4f& w);

        /**
         * Pre-multiple a matrix-b to matrix-a
         * @param a
         * @param b
         * @param result
         */
        static void Multipy(const Matrix& a, const Matrix& b, Matrix& result);

        /**
         * Pre-multiple a matrix to this
         * A.operator* B means B * A
         * @param other
         * @return
         */

        float& At(int32 row, int32 column)
        {
            ENSURE(0 <= row && row < 4 && 0 <= column && column < 4);
            return M[row][column];
        }

        float At(int32 row, int32 column) const
        {
            ENSURE(0 <= row && row < 4 && 0 <= column && column < 4);
            return M[row][column];
        }

        void SetIdentity();

        Vector3f GetScale(float tolerance = SMALL_FLOAT) const;

        Matrix operator* (const Matrix& other) const;

        Matrix operator*= (const Matrix& other);

        Vector4f operator[] (int32 row) const;

        static const Matrix Identity;

        float M[4][4];
    };
}