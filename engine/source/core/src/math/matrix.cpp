#include "math/matrix.hpp"

namespace Engine
{
    const Matrix Matrix::Identity(
        Vector4f(1, 0, 0, 0),
        Vector4f(0, 1, 0, 0),
        Vector4f(0, 0, 1, 0),
        Vector4f(0, 0, 0, 1)
    );

    Matrix::Matrix(const Vector4f& x, const Vector4f& y, const Vector4f& z, const Vector4f& w)
    {
        Memory::Memcpy(M[0], const_cast<float*>(&x.X), sizeof(float) * 4);
        Memory::Memcpy(M[1], const_cast<float*>(&y.X), sizeof(float) * 4);
        Memory::Memcpy(M[2], const_cast<float*>(&z.X), sizeof(float) * 4);
        Memory::Memcpy(M[3], const_cast<float*>(&w.X), sizeof(float) * 4);
    }

    void Matrix::Multipy(const Matrix& a, const Matrix& b, Matrix& result)
    {
        Vector4f row0(
            a.M[0][0] * b.M[0][0] + a.M[1][0] * b.M[0][1] + a.M[2][0] * b.M[0][2] + a.M[3][0] * b.M[0][3],
            a.M[0][1] * b.M[0][0] + a.M[1][1] * b.M[0][1] + a.M[2][1] * b.M[0][2] + a.M[3][1] * b.M[0][3],
            a.M[0][2] * b.M[0][0] + a.M[1][2] * b.M[0][1] + a.M[2][2] * b.M[0][2] + a.M[3][2] * b.M[0][3],
            a.M[0][3] * b.M[0][0] + a.M[1][3] * b.M[0][1] + a.M[2][3] * b.M[0][2] + a.M[3][3] * b.M[0][3]);

        Vector4f row1(
            a.M[0][0] * b.M[1][0] + a.M[1][0] * b.M[1][1] + a.M[2][0] * b.M[1][2] + a.M[3][0] * b.M[1][3],
            a.M[0][1] * b.M[1][0] + a.M[1][1] * b.M[1][1] + a.M[2][1] * b.M[1][2] + a.M[3][1] * b.M[1][3],
            a.M[0][2] * b.M[1][0] + a.M[1][2] * b.M[1][1] + a.M[2][2] * b.M[1][2] + a.M[3][2] * b.M[1][3],
            a.M[0][3] * b.M[1][0] + a.M[1][3] * b.M[1][1] + a.M[2][3] * b.M[1][2] + a.M[3][3] * b.M[1][3]);

        Vector4f row2(
            a.M[0][0] * b.M[2][0] + a.M[1][0] * b.M[2][1] + a.M[2][0] * b.M[2][2] + a.M[3][0] * b.M[2][3],
            a.M[0][1] * b.M[2][0] + a.M[1][1] * b.M[2][1] + a.M[2][1] * b.M[2][2] + a.M[3][1] * b.M[2][3],
            a.M[0][2] * b.M[2][0] + a.M[1][2] * b.M[2][1] + a.M[2][2] * b.M[2][2] + a.M[3][2] * b.M[2][3],
            a.M[0][3] * b.M[2][0] + a.M[1][3] * b.M[2][1] + a.M[2][3] * b.M[2][2] + a.M[3][3] * b.M[2][3]);

        Vector4f row3(
            a.M[0][0] * b.M[3][0] + a.M[1][0] * b.M[3][1] + a.M[2][0] * b.M[3][2] + a.M[3][0] * b.M[3][3],
            a.M[0][1] * b.M[3][0] + a.M[1][1] * b.M[3][1] + a.M[2][1] * b.M[3][2] + a.M[3][1] * b.M[3][3],
            a.M[0][2] * b.M[3][0] + a.M[1][2] * b.M[3][1] + a.M[2][2] * b.M[3][2] + a.M[3][2] * b.M[3][3],
            a.M[0][3] * b.M[3][0] + a.M[1][3] * b.M[3][1] + a.M[2][3] * b.M[3][2] + a.M[3][3] * b.M[3][3]);

        result = Matrix(row0, row1, row2, row3);
    }

    void Matrix::SetIdentity()
    {
        M[0][0] = 1; M[0][1] = 0;  M[0][2] = 0;  M[0][3] = 0;
        M[1][0] = 0; M[1][1] = 1;  M[1][2] = 0;  M[1][3] = 0;
        M[2][0] = 0; M[2][1] = 0;  M[2][2] = 1;  M[2][3] = 0;
        M[3][0] = 0; M[3][1] = 0;  M[3][2] = 0;  M[3][3] = 1;
    }

    Vector3f Matrix::GetScale(float tolerance) const
    {
        Vector3f scale;
        for (int32 i = 0; i < 3; ++i)
        {
            float lengthSq = M[i][0] * M[i][0] + M[i][1] * M[i][1] + M[i][2] * M[i][2];
            if (lengthSq <= tolerance)
            {
                scale[i] = 0;
            }
            else
            {
                scale[i] = Math::Sqrt(lengthSq);
            }
        }

        return scale;
    }

    Matrix Matrix::operator*(const Matrix& other) const
    {
        Matrix result;
        Multipy(other, *this, result);
        return result;
    }

    Matrix Matrix::operator*=(const Matrix& other)
    {
        Multipy(other, *this, *this);
        return *this;
    }

    Vector4f Matrix::operator[](int32 row) const
    {
        return Vector4f(M[row][0], M[row][1], M[row][2], M[row][3]);
    }
}