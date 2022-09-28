export void RotatorToQuat(const uniform float rotator[3], uniform float quat[4])
{
    uniform float compSin[3]; // Store as pitch yaw roll
    uniform float compCos[3]; // Store as pitch yaw roll

    for (uniform int index = 0; index < 3; ++index)
    {
        sincos(rotator[index] * PI / 360.f, &compSin[index], &compCos[index]);
    }

    quat[0] = compCos[2] * compSin[0] * compSin[1] - compSin[2] * compCos[0] * compCos[1];
    quat[1] = -compCos[2] * compSin[0] * compCos[1] - compSin[2] * compCos[0] * compSin[1];
    quat[2] = compCos[2] * compCos[0] * compSin[1] - compSin[2] * compSin[0] * compCos[1];
    quat[3] = compCos[2] * compCos[0] * compCos[1] + compSin[2] * compSin[0] * compSin[1];
}