export void FloatNormalize(uniform float* uniform sequence, uniform int size)
{
    uniform float sq = 0.0f;
    for (uniform int index = 0; index < size; ++index)
    {
        sq = sq + sequence[index] * sequence[index];
    }

    float scale = rsqrt(sq);

    foreach (index = 0 ... size)
    {
        sequence[index] = sequence[index] * scale;
    }
}

export void FloatNormalizeFast(uniform float* uniform sequence, uniform int size)
{
    uniform float sq = 0.0f;
    for (uniform int index = 0; index < size; ++index)
    {
        sq = sq + sequence[index] * sequence[index];
    }

    float scale = rsqrt_fast(sq);

    foreach (index = 0 ... size)
    {
        sequence[index] = sequence[index] * scale;
    }
}