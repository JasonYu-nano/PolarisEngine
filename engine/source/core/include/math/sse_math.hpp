#pragma once

#include "global.hpp"

#if SUPPORT_SSE42
#include <nmmintrin.h>
#elif SUPPORT_SSE41
#include <smmintrin.h>
#elif SUPPORT_SSSE3
#include <tmmintrin.h>
#elif SUPPORT_SSE3
#include <pmmintrin.h>
#elif SUPPORT_SSE2
#include <emmintrin.h>
#elif SUPPORT_SSE
#include <xmmintrin.h>
#endif

namespace Engine
{
    using VectorRegister = __m128;

    inline VectorRegister MakeVectorRegister(float x)
    {
        return _mm_setr_ps(x, x, x, x);
    }

    inline VectorRegister MakeVectorRegister(float x, float y, float z, float w)
    {
        return _mm_setr_ps(x, y, z, w);
    }

    inline VectorRegister VectorShuffle(VectorRegister vec, int32 a, int32 b, int32 c, int32 d)
    {
        return _mm_shuffle_ps(vec, vec, _MM_SHUFFLE(a, b, c, d));
    }

    inline VectorRegister VectorShuffle(VectorRegister vec1, VectorRegister vec2, int32 a, int32 b, int32 c, int32 d)
    {
        return _mm_shuffle_ps(vec1, vec2, _MM_SHUFFLE(a, b, c, d));
    }

    inline VectorRegister Vector3fDot(const VectorRegister& lhs, const VectorRegister& rhs)
    {
#if !SUPPORT_SSE41
        return _mm_dp_ps(lhs, rhs, 0x7f);
#else
        VectorRegister dot = _mm_mul_ps(lhs, rhs);
        VectorRegister temp = VectorShuffle(dot, 1, 1, 2, 2);
        dot = _mm_add_ps(dot, temp);
        temp = VectorShuffle(dot, 3, 3, 3, 3);
        dot = _mm_add_ps(dot, temp);
        return VectorShuffle(dot, 0, 0, 0, 0);
#endif
    }

    /**
     * Return (0, 0, 0, 0)
     * @return
     */
    inline VectorRegister VectorZero()
    {
        return _mm_setzero_ps();
    }

    inline VectorRegister VectorSetW(VectorRegister vector, float w)
    {
        VectorRegister temp = _mm_movehl_ps(MakeVectorRegister(w, w, w, w), vector);
        return VectorShuffle(vector, temp, 0, 1, 0, 3);
    }
}