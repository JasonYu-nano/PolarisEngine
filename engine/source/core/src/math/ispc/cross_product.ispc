export void CrossProduct(uniform const float a[3], uniform const float b[3], uniform float result[3])
{
    foreach(index = 0 ... 3)
    {
        int indexA = ((index + 1 == 3) ? 0 : index + 1);
        int indexB = ((index == 0) ? 2 : index - 1);
        result[index] = a[indexA] * b[indexB] - a[indexB] * b[indexA];
    }
}